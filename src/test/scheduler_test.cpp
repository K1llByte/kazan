#include "core/flat_map.hpp"
#include "ecs/scene.hpp"
#include "ecs/scheduler.hpp"

#include <chrono>
#include <iostream>
#include <cstdint>

using namespace kzn;

///////////////////////////////////////////////////////////////////////////////
// Scheduler using FlatMap
///////////////////////////////////////////////////////////////////////////////

class FlatMapExecutor {
public:
    explicit FlatMapExecutor(std::vector<System*> order)
        : m_execution_order(std::move(order)) {}

    FlatMapExecutor(const FlatMapExecutor&) = delete;
    FlatMapExecutor& operator=(const FlatMapExecutor&) = delete;

    FlatMapExecutor(FlatMapExecutor&&) = default;
    FlatMapExecutor& operator=(FlatMapExecutor&&) = default;

    ~FlatMapExecutor() = default;

    void update(Scene& scene, float delta_time) {
        for (System* sys : m_execution_order) {
            sys->update(delta_time);
        }
    }

private:
    std::vector<System*> m_execution_order;
};

class FlatMapScheduler {
public:
    FlatMapScheduler() = default;

    FlatMapScheduler(const FlatMapScheduler&) = delete;
    FlatMapScheduler& operator=(const FlatMapScheduler&) = delete;

    FlatMapScheduler(FlatMapScheduler&&) = default;
    FlatMapScheduler& operator=(FlatMapScheduler&&) = default;

    ~FlatMapScheduler() = default;

    template<typename S, typename... Args>
        requires std::is_base_of_v<System, S>
    constexpr S& emplace(Args&&... args) {
        const std::type_index type_id = typeid(S);

        KZN_ASSERT_MSG(!m_systems.contains(type_id),
                       "System already registered");

        auto ptr = std::make_unique<S>(std::forward<Args>(args)...);
        S& ref = *ptr;

        m_systems.emplace(type_id, std::move(ptr));
        m_edges.emplace(type_id, std::vector<std::type_index>{});

        if constexpr (requires { typename S::Before; }) {
            [this]<typename... Ts>(TypeList<Ts...>) {
                (before<S, Ts>(), ...);
            }(typename S::Before{});
        }

        return ref;
    }

    template<typename Before, typename After>
    constexpr void before() {
        auto& edges = m_edges[typeid(Before)];

        KZN_ASSERT_MSG(
            std::find(edges.begin(), edges.end(), typeid(After)) == edges.end(),
            "Edge already exists"
        );

        edges.push_back(typeid(After));
    }

    template<typename After, typename Before>
    constexpr void after() {
        before<Before, After>();
    }

    [[nodiscard]]
    constexpr FlatMapExecutor build() {
        return FlatMapExecutor(topological_sort());
    }

private:
    [[nodiscard]]
    constexpr std::vector<System*> topological_sort() {
        FlatMap<std::type_index, std::size_t> incoming_edges_count;

        // Initialize incoming edge counts
        for (auto& [node, _] : m_edges) {
            incoming_edges_count.emplace(node, 0);
        }

        // Count incoming edges
        for (auto& [_, tos] : m_edges) {
            for (auto to : tos) {
                incoming_edges_count[to] += 1;
            }
        }

        std::queue<std::type_index> ready_nodes;
        for (auto& [node, count] : incoming_edges_count) {
            if (count == 0) {
                ready_nodes.push(node);
            }
        }

        std::vector<System*> result;
        result.reserve(m_systems.size());

        while (!ready_nodes.empty()) {
            auto node = ready_nodes.front();
            ready_nodes.pop();

            result.push_back(m_systems.at(node).get());

            for (auto& to : m_edges[node]) {
                if (--incoming_edges_count[to] == 0) {
                    ready_nodes.push(to);
                }
            }
        }

        if (result.size() != m_systems.size()) {
            throw std::runtime_error(
                "Topological sort not possible due to cyclic dependencies"
            );
        }

        return result;
    }

private:
    FlatMap<std::type_index, std::unique_ptr<System>> m_systems;
    FlatMap<std::type_index, std::vector<std::type_index>> m_edges;
};

///////////////////////////////////////////////////////////////////////////////
// Scheduler using std::vector
///////////////////////////////////////////////////////////////////////////////

class VectorExecutor {
public:
    // Ctor
    explicit VectorExecutor(std::vector<System*> order)
        : m_execution_order(std::move(order)) {}

    // Copy
    VectorExecutor(const VectorExecutor&) = delete;
    VectorExecutor& operator=(const VectorExecutor&) = delete;

    // Move
    VectorExecutor(VectorExecutor&&) = default;
    VectorExecutor& operator=(VectorExecutor&&) = default;

    // Dtor
    ~VectorExecutor() = default;

    //! Update all systems in order.
    void update(Scene& scene, float delta_time) {
        for (System* sys : m_execution_order) {
            sys->update(scene, delta_time);
        }
    }

private:
    std::vector<System*> m_execution_order;
};

class VectorScheduler {
public:
    VectorScheduler() = default;

    VectorScheduler(const VectorScheduler&) = delete;
    VectorScheduler& operator=(const VectorScheduler&) = delete;

    VectorScheduler(VectorScheduler&&) = default;
    VectorScheduler& operator=(VectorScheduler&&) = default;

    ~VectorScheduler() = default;

    template<typename S, typename... Args>
        requires std::is_base_of_v<System, S>
    constexpr S& emplace(Args&&... args) {
        const std::type_index type_id = typeid(S);

        KZN_ASSERT_MSG(
            std::ranges::find_if(
                m_systems,
                [&](const SystemEntry& e) { return e.first == type_id; }
            ) == m_systems.end(),
            "System already registered"
        );

        auto ptr = std::make_unique<S>(std::forward<Args>(args)...);
        S& ref = *ptr;

        m_systems.emplace_back(type_id, std::move(ptr));
        m_edges.emplace_back(type_id, std::vector<std::type_index>{});

        if constexpr (requires { typename S::Before; }) {
            [this]<typename... Ts>(TypeList<Ts...>) {
                (before<S, Ts>(), ...);
            }(typename S::Before{});
        }

        return ref;
    }

    template<typename Before, typename After>
    constexpr void before() {
        auto it = std::ranges::find_if(
            m_edges,
            [&](const EdgeEntry& e) { return e.first == typeid(Before); }
        );
        KZN_ASSERT_MSG(it != m_edges.end(), "Before system not registered");

        auto& edges = it->second;

        KZN_ASSERT_MSG(
            std::find(edges.begin(), edges.end(), typeid(After)) == edges.end(),
            "Edge already exists"
        );

        edges.push_back(typeid(After));
    }

    template<typename After, typename Before>
    constexpr void after() {
        before<Before, After>();
    }

    [[nodiscard]]
    constexpr VectorExecutor build() {
        auto order = topological_sort();
        return VectorExecutor(std::move(order));
    }

private:
    using SystemEntry = std::pair<std::type_index, std::unique_ptr<System>>;
    using EdgeEntry   = std::pair<std::type_index, std::vector<std::type_index>>;

    [[nodiscard]]
    constexpr std::vector<System*> topological_sort() {
        std::vector<std::pair<std::type_index, std::size_t>> incoming;

        // Initialize incoming edge counts
        for (auto& [node, _] : m_edges) {
            incoming.emplace_back(node, 0);
        }

        auto find_incoming = [&](std::type_index id) {
            return std::find_if(
                incoming.begin(),
                incoming.end(),
                [&](auto& p) { return p.first == id; }
            );
        };

        // Count incoming edges
        for (auto& [_, tos] : m_edges) {
            for (auto to : tos) {
                auto it = find_incoming(to);
                KZN_ASSERT_MSG(it != incoming.end(), "Edge references unknown system");
                ++it->second;
            }
        }

        std::queue<std::type_index> ready;
        for (auto& [node, count] : incoming) {
            if (count == 0) {
                ready.push(node);
            }
        }

        std::vector<System*> result;

        while (!ready.empty()) {
            auto node = ready.front();
            ready.pop();

            
            auto sys_it = std::ranges::find_if(
                m_systems,
                [&](const SystemEntry& e) { return e.first == node; }
            );
            KZN_ASSERT_MSG(sys_it != m_systems.end(), "System missing");
            result.push_back(sys_it->second.get());

            auto edge_it = std::ranges::find_if(
                m_edges,
                [&](const EdgeEntry& e) { return e.first == node; }
            );
            for (auto& to : edge_it->second) {
                auto inc_it = find_incoming(to);
                if (--inc_it->second == 0) {
                    ready.push(to);
                }
            }
        }

        if (result.size() != m_systems.size()) {
            throw std::runtime_error(
                "Topological sort not possible due to cyclic dependencies"
            );
        }

        return result;
    }

private:
    std::vector<SystemEntry> m_systems;
    std::vector<EdgeEntry>   m_edges;
};

///////////////////////////////////////////////////////////////////////////////
// Dummy Systems for this test
///////////////////////////////////////////////////////////////////////////////

// 100 empty systems
struct System0 : System {};
struct System1 : System {};
struct System2 : System {};
struct System3 : System {};
struct System4 : System {};
struct System5 : System {};
struct System6 : System {};
struct System7 : System {};
struct System8 : System {};
struct System9 : System {};
struct System10 : System {};
struct System11 : System {};
struct System12 : System {};
struct System13 : System {};
struct System14 : System {};
struct System15 : System {};
struct System16 : System {};
struct System17 : System {};
struct System18 : System {};
struct System19 : System {};
struct System20 : System {};
struct System21 : System {};
struct System22 : System {};
struct System23 : System {};
struct System24 : System {};
struct System25 : System {};
struct System26 : System {};
struct System27 : System {};
struct System28 : System {};
struct System29 : System {};
struct System30 : System {};
struct System31 : System {};
struct System32 : System {};
struct System33 : System {};
struct System34 : System {};
struct System35 : System {};
struct System36 : System {};
struct System37 : System {};
struct System38 : System {};
struct System39 : System {};
struct System40 : System {};
struct System41 : System {};
struct System42 : System {};
struct System43 : System {};
struct System44 : System {};
struct System45 : System {};
struct System46 : System {};
struct System47 : System {};
struct System48 : System {};
struct System49 : System {};
struct System50 : System {};
struct System51 : System {};
struct System52 : System {};
struct System53 : System {};
struct System54 : System {};
struct System55 : System {};
struct System56 : System {};
struct System57 : System {};
struct System58 : System {};
struct System59 : System {};
struct System60 : System {};
struct System61 : System {};
struct System62 : System {};
struct System63 : System {};
struct System64 : System {};
struct System65 : System {};
struct System66 : System {};
struct System67 : System {};
struct System68 : System {};
struct System69 : System {};
struct System70 : System {};
struct System71 : System {};
struct System72 : System {};
struct System73 : System {};
struct System74 : System {};
struct System75 : System {};
struct System76 : System {};
struct System77 : System {};
struct System78 : System {};
struct System79 : System {};
struct System80 : System {};
struct System81 : System {};
struct System82 : System {};
struct System83 : System {};
struct System84 : System {};
struct System85 : System {};
struct System86 : System {};
struct System87 : System {};
struct System88 : System {};
struct System89 : System {};
struct System90 : System {};
struct System91 : System {};
struct System92 : System {};
struct System93 : System {};
struct System94 : System {};
struct System95 : System {};
struct System96 : System {};
struct System97 : System {};
struct System98 : System {};
struct System99 : System {};


template<typename S>
void benchmark100(const std::string_view name, std::size_t iterations) {
    S scheduler;

    // Emplace systems (random order)
    scheduler.template emplace<System42>();
    scheduler.template emplace<System7>();
    scheduler.template emplace<System89>();
    scheduler.template emplace<System15>();
    scheduler.template emplace<System63>();
    scheduler.template emplace<System28>();
    scheduler.template emplace<System91>();
    scheduler.template emplace<System3>();
    scheduler.template emplace<System56>();
    scheduler.template emplace<System74>();
    scheduler.template emplace<System18>();
    scheduler.template emplace<System99>();
    scheduler.template emplace<System34>();
    scheduler.template emplace<System60>();
    scheduler.template emplace<System43>();
    scheduler.template emplace<System12>();
    scheduler.template emplace<System85>();
    scheduler.template emplace<System47>();
    scheduler.template emplace<System5>();
    scheduler.template emplace<System68>();
    scheduler.template emplace<System26>();
    scheduler.template emplace<System80>();
    scheduler.template emplace<System1>();
    scheduler.template emplace<System52>();
    scheduler.template emplace<System39>();
    scheduler.template emplace<System97>();
    scheduler.template emplace<System21>();
    scheduler.template emplace<System66>();
    scheduler.template emplace<System9>();
    scheduler.template emplace<System44>();
    scheduler.template emplace<System71>();
    scheduler.template emplace<System31>();
    scheduler.template emplace<System58>();
    scheduler.template emplace<System0>();
    scheduler.template emplace<System83>();
    scheduler.template emplace<System14>();
    scheduler.template emplace<System49>();
    scheduler.template emplace<System92>();
    scheduler.template emplace<System23>();
    scheduler.template emplace<System65>();
    scheduler.template emplace<System10>();
    scheduler.template emplace<System76>();
    scheduler.template emplace<System55>();
    scheduler.template emplace<System36>();
    scheduler.template emplace<System87>();
    scheduler.template emplace<System2>();
    scheduler.template emplace<System90>();
    scheduler.template emplace<System20>();
    scheduler.template emplace<System61>();
    scheduler.template emplace<System33>();
    scheduler.template emplace<System72>();
    scheduler.template emplace<System16>();
    scheduler.template emplace<System98>();
    scheduler.template emplace<System46>();
    scheduler.template emplace<System27>();
    scheduler.template emplace<System79>();
    scheduler.template emplace<System4>();
    scheduler.template emplace<System69>();
    scheduler.template emplace<System41>();
    scheduler.template emplace<System95>();
    scheduler.template emplace<System24>();
    scheduler.template emplace<System54>();
    scheduler.template emplace<System11>();
    scheduler.template emplace<System82>();
    scheduler.template emplace<System64>();
    scheduler.template emplace<System35>();
    scheduler.template emplace<System88>();
    scheduler.template emplace<System8>();
    scheduler.template emplace<System77>();
    scheduler.template emplace<System30>();
    scheduler.template emplace<System57>();
    scheduler.template emplace<System94>();
    scheduler.template emplace<System19>();
    scheduler.template emplace<System62>();
    scheduler.template emplace<System25>();
    scheduler.template emplace<System81>();
    scheduler.template emplace<System6>();
    scheduler.template emplace<System73>();
    scheduler.template emplace<System48>();
    scheduler.template emplace<System93>();
    scheduler.template emplace<System22>();
    scheduler.template emplace<System67>();
    scheduler.template emplace<System13>();
    scheduler.template emplace<System84>();
    scheduler.template emplace<System38>();
    scheduler.template emplace<System51>();
    scheduler.template emplace<System96>();
    scheduler.template emplace<System29>();
    scheduler.template emplace<System70>();
    scheduler.template emplace<System45>();
    scheduler.template emplace<System86>();
    scheduler.template emplace<System17>();
    scheduler.template emplace<System78>();
    scheduler.template emplace<System40>();
    scheduler.template emplace<System59>();
    scheduler.template emplace<System32>();
    scheduler.template emplace<System75>();
    scheduler.template emplace<System53>();
    scheduler.template emplace<System37>();
    scheduler.template emplace<System50>();

    // Linear dependency chain (99 edges)
    scheduler.template before<System0, System1>();
    scheduler.template before<System1, System2>();
    scheduler.template before<System2, System3>();
    scheduler.template before<System3, System4>();
    scheduler.template before<System4, System5>();
    scheduler.template before<System5, System6>();
    scheduler.template before<System6, System7>();
    scheduler.template before<System7, System8>();
    scheduler.template before<System8, System9>();
    scheduler.template before<System9, System10>();
    scheduler.template before<System10, System11>();
    scheduler.template before<System11, System12>();
    scheduler.template before<System12, System13>();
    scheduler.template before<System13, System14>();
    scheduler.template before<System14, System15>();
    scheduler.template before<System15, System16>();
    scheduler.template before<System16, System17>();
    scheduler.template before<System17, System18>();
    scheduler.template before<System18, System19>();
    scheduler.template before<System19, System20>();
    scheduler.template before<System20, System21>();
    scheduler.template before<System21, System22>();
    scheduler.template before<System22, System23>();
    scheduler.template before<System23, System24>();
    scheduler.template before<System24, System25>();
    scheduler.template before<System25, System26>();
    scheduler.template before<System26, System27>();
    scheduler.template before<System27, System28>();
    scheduler.template before<System28, System29>();
    scheduler.template before<System29, System30>();
    scheduler.template before<System30, System31>();
    scheduler.template before<System31, System32>();
    scheduler.template before<System32, System33>();
    scheduler.template before<System33, System34>();
    scheduler.template before<System34, System35>();
    scheduler.template before<System35, System36>();
    scheduler.template before<System36, System37>();
    scheduler.template before<System37, System38>();
    scheduler.template before<System38, System39>();
    scheduler.template before<System39, System40>();
    scheduler.template before<System40, System41>();
    scheduler.template before<System41, System42>();
    scheduler.template before<System42, System43>();
    scheduler.template before<System43, System44>();
    scheduler.template before<System44, System45>();
    scheduler.template before<System45, System46>();
    scheduler.template before<System46, System47>();
    scheduler.template before<System47, System48>();
    scheduler.template before<System48, System49>();
    scheduler.template before<System49, System50>();
    scheduler.template before<System50, System51>();
    scheduler.template before<System51, System52>();
    scheduler.template before<System52, System53>();
    scheduler.template before<System53, System54>();
    scheduler.template before<System54, System55>();
    scheduler.template before<System55, System56>();
    scheduler.template before<System56, System57>();
    scheduler.template before<System57, System58>();
    scheduler.template before<System58, System59>();
    scheduler.template before<System59, System60>();
    scheduler.template before<System60, System61>();
    scheduler.template before<System61, System62>();
    scheduler.template before<System62, System63>();
    scheduler.template before<System63, System64>();
    scheduler.template before<System64, System65>();
    scheduler.template before<System65, System66>();
    scheduler.template before<System66, System67>();
    scheduler.template before<System67, System68>();
    scheduler.template before<System68, System69>();
    scheduler.template before<System69, System70>();
    scheduler.template before<System70, System71>();
    scheduler.template before<System71, System72>();
    scheduler.template before<System72, System73>();
    scheduler.template before<System73, System74>();
    scheduler.template before<System74, System75>();
    scheduler.template before<System75, System76>();
    scheduler.template before<System76, System77>();
    scheduler.template before<System77, System78>();
    scheduler.template before<System78, System79>();
    scheduler.template before<System79, System80>();
    scheduler.template before<System80, System81>();
    scheduler.template before<System81, System82>();
    scheduler.template before<System82, System83>();
    scheduler.template before<System83, System84>();
    scheduler.template before<System84, System85>();
    scheduler.template before<System85, System86>();
    scheduler.template before<System86, System87>();
    scheduler.template before<System87, System88>();
    scheduler.template before<System88, System89>();
    scheduler.template before<System89, System90>();
    scheduler.template before<System90, System91>();
    scheduler.template before<System91, System92>();
    scheduler.template before<System92, System93>();
    scheduler.template before<System93, System94>();
    scheduler.template before<System94, System95>();
    scheduler.template before<System95, System96>();
    scheduler.template before<System96, System97>();
    scheduler.template before<System97, System98>();
    scheduler.template before<System98, System99>();

    std::size_t sink = 0;

    auto start = std::chrono::high_resolution_clock::now();
    for (std::size_t i = 0; i < iterations; ++i) {
        auto executor = scheduler.build();
        sink += reinterpret_cast<std::uintptr_t>(&executor);
    }
    auto end = std::chrono::high_resolution_clock::now();

    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    std::cout << name << ": "
              << (ns / iterations) << " ns/build\n";
}

template<typename S>
void benchmark20(const std::string_view name, std::size_t iterations) {
    S scheduler;

    // Emplace systems (random order)
    scheduler.template emplace<System7>();
    scheduler.template emplace<System12>();
    scheduler.template emplace<System3>();
    scheduler.template emplace<System18>();
    scheduler.template emplace<System0>();
    scheduler.template emplace<System15>();
    scheduler.template emplace<System9>();
    scheduler.template emplace<System5>();
    scheduler.template emplace<System14>();
    scheduler.template emplace<System1>();
    scheduler.template emplace<System19>();
    scheduler.template emplace<System6>();
    scheduler.template emplace<System10>();
    scheduler.template emplace<System4>();
    scheduler.template emplace<System16>();
    scheduler.template emplace<System2>();
    scheduler.template emplace<System8>();
    scheduler.template emplace<System11>();
    scheduler.template emplace<System17>();
    scheduler.template emplace<System13>();

    // Linear dependency chain (19 edges)
    scheduler.template before<System0, System1>();
    scheduler.template before<System1, System2>();
    scheduler.template before<System2, System3>();
    scheduler.template before<System3, System4>();
    scheduler.template before<System4, System5>();
    scheduler.template before<System5, System6>();
    scheduler.template before<System6, System7>();
    scheduler.template before<System7, System8>();
    scheduler.template before<System8, System9>();
    scheduler.template before<System9, System10>();
    scheduler.template before<System10, System11>();
    scheduler.template before<System11, System12>();
    scheduler.template before<System12, System13>();
    scheduler.template before<System13, System14>();
    scheduler.template before<System14, System15>();
    scheduler.template before<System15, System16>();
    scheduler.template before<System16, System17>();
    scheduler.template before<System17, System18>();
    scheduler.template before<System18, System19>();

    std::size_t sink = 0;

    auto start = std::chrono::high_resolution_clock::now();
    for (std::size_t i = 0; i < iterations; ++i) {
        auto executor = scheduler.build();
        sink += reinterpret_cast<std::uintptr_t>(&executor);
    }
    auto end = std::chrono::high_resolution_clock::now();

    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    std::cout << name << ": "
              << (ns / iterations) << " ns/build\n";
}

int main() {
    constexpr std::size_t iterations = 1000;

    benchmark100<Scheduler>("MapScheduler (100 systems)", iterations);
    benchmark100<VectorScheduler>("VectorScheduler (100 systems)", iterations);
    benchmark100<FlatMapScheduler>("FlatMapScheduler (100 systems)", iterations);

    benchmark20<Scheduler>("MapScheduler (20 systems)", iterations);
    benchmark20<VectorScheduler>("VectorScheduler (20 systems)", iterations);
    benchmark20<FlatMapScheduler>("FlatMapScheduler (20 systems)", iterations);

    return 0;
}