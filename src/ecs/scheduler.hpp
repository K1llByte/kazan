#pragma once

#include "core/assert.hpp"
#include "ecs/system.hpp"

#include <cstddef>
#include <memory>
#include <queue>
#include <stdexcept>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace kzn {

//! Executes ECS systems in a precomputed order.
//! The `Executor` contains a finalized execution schedule usually produced by
//! the `Scheduler`. It contains an ordered list of system pointers and it's
//! responsible for invoking `update()` on each system.
//!
//! The execution order is fixed at construction time and cannot be modified.
//! The `Executor` does not own the systems it executes, it assumes that all
//! referenced systems must outlive the lifetime of the executor.
//!
//! Typical usage involves repeatedly calling `update()` once per frame or
//! simulation step.
//!
//! \note `Executor` instances are moveable but not copyable.
//! \note The execution order is guaranteed to respect all dependencies
//! declared in the originating `Scheduler`.
class Executor {
public:
    // Ctor
    explicit Executor(std::vector<System*> order)
        : m_execution_order(std::move(order)) {}

    // Copy
    Executor(const Executor&) = delete;
    Executor& operator=(const Executor&) = delete;

    // Move
    Executor(Executor&&) = default;
    Executor& operator=(Executor&&) = default;

    // Dtor
    ~Executor() = default;

    //! Update all systems in order.
    void update(float delta_time) {
        for (System* sys : m_execution_order) {
            sys->update(delta_time);
        }
    }

private:
    std::vector<System*> m_execution_order;
};

//! Schedules and orders ECS systems for execution.
//!
//! The `Scheduler` is responsible for registering systems, declaring
//! execution-order dependencies between systems and producing an executable
//! schedule that respects those dependencies.
//!
//! Systems are registered by type using `emplace()`, and execution
//! constraints are expressed via `before()` and `after()`. Internally,
//! the scheduler builds a directed acyclic graph (DAG) of system
//! dependencies and resolves it using a topological sort.
//!
//! Once all systems and dependencies are declared, `build()` is called
//! to produce an `Executor`, which represents the finalized execution order
//! and can be used to update systems repeatedly.
//!
//! The `Scheduler` itself does not execute systems and does not own
//! runtime execution state beyond system registration and dependency
//! tracking.
//!
//! \note Each system type may be registered only once.
//! \note Cyclic dependencies between systems are detected at build time
//!       and an exception will be thrown.
//! \note The relative execution order of systems with no dependency
//!       relationship is unspecified.
//!
//! \example
//! \code
//! auto scheduler = kzn::Scheduler();
//! scheduler.emplace<FooSystem>();
//! scheduler.emplace<BarSystem>();
//! scheduler.before<BarSystem, FooSystem>();
//!
//! auto executor = scheduler.build();
//! // BarSystem will call update before FooSystem
//! executor.update(delta_time);
//! \endcode
class Scheduler {
public:
    // Ctor
    Scheduler() = default;

    // Copy
    Scheduler(const Scheduler&) = delete;
    Scheduler& operator=(const Scheduler&) = delete;

    // Move
    Scheduler(Scheduler&&) = default;
    Scheduler& operator=(Scheduler&&) = default;

    // Dtor
    ~Scheduler() = default;

    //! Constructs a system of type `S` in-lace and registers it with the
    //! scheduler. Each system type may be registered only once.
    //!
    //! Registered systems participate in dependency resolution and execution
    //! ordering when `build()` is called.
    //!
    //! \tparam S    The concrete system type to register. Must derive from
    //!              `System`.
    //! \tparam Args Constructor argument types forwarded to `S`.
    //!
    //! \param args  Arguments forwarded to the constructor of `S`.
    //!
    //! \return A reference to the newly created system instance.
    //!
    //! \throws std::runtime_error
    //!         Thrown if a system of type `S` has already been registered.
    //!
    //! \note The returned reference remains valid until the scheduler is
    //!       destroyed or the system is otherwise removed.
    //! \note Systems must be registered before declaring dependencies using
    //!       `before()` or `after()`.
    //! \note Registering a system implicitly creates a node in the internal
    //!       dependency graph, even if no dependencies are declared.
    template<typename S, typename... Args>
        requires std::is_base_of_v<System, S>
    constexpr S& emplace(Args&&... args) {
        const std::type_index type_id = typeid(S);

        KZN_ASSERT_MSG(!m_systems.count(type_id), "System already registered");

        auto ptr = std::make_unique<S>(std::forward<Args>(args)...);
        S& ref = *ptr;

        // Add an entry in m_systems
        m_systems[type_id] = std::move(ptr);
        // Add an entry in m_edges
        m_edges[type_id];

        return ref;
    }

    //! Declare an execution dependency between two systems.
    //! Ensures that the system of type `Before` is executed *before*
    //! the system of type `After` when the scheduler is built.
    //! This creates a directed edge in the internal dependency graph:
    //!     Before -> After
    //!
    //! \tparam Before The system that must execute first.
    //! \tparam After  The system that must execute after `Before`.
    //! \note Both systems must be registered in the scheduler before calling
    //! this function.
    template<typename Before, typename After>
    constexpr void before() {
        auto& edges = m_edges[typeid(Before)];

        KZN_ASSERT_MSG(
            std::find(edges.begin(), edges.end(), typeid(After)) == edges.end(),
            "Edge already exists"
        );
        // FIXME: This needs to be checked in build(), not here.
        // KZN_ASSERT_MSG(
        //     m_systems.contains(typeid(Before)) &&
        //         m_systems.contains(typeid(After)),
        //     "System does not exist"
        // );

        edges.push_back(typeid(After));
    }

    //! Declare an execution dependency using inverted wording.
    //! Ensures that the system of type `After` is executed *after*
    //! the system of type `Before` when the scheduler is built.
    //! This function is equivalent to calling:
    //!     before<Before, After>()
    //!
    //! \tparam After  The system that must execute later.
    //! \tparam Before The system that must execute earlier.
    //! @note Both systems must be registered with the scheduler via
    //!       `emplace()` before calling this function.
    template<typename After, typename Before>
    constexpr void after() {
        before<Before, After>();
    }

    //! Build an executor from the scheduler with the currently registered
    //! systems and dependencies.
    //! Resolves all declared system dependencies and computes a valid execution
    //! order using a topological sort of the dependency graph.
    //! The returned `Executor` contains the finalized execution order and can
    //! be used to repeatedly update all systems in the correct order.
    //!
    //! \return An `Executor` containing the ordered list of systems to execute.
    //! \throws std::runtime_error if the dependency graph contains a cycle
    //!         or if no valid execution order can be produced.
    //! \note All systems must be registered via `emplace()` before calling
    //!       this function. Once built, changes to the scheduler will not
    //!       affect the returned `Executor`.
    //! \note Calling `build()` multiple times will recompute the schedule
    //!       but will not invalidate previously returned `Executor` instances.
    [[nodiscard]]
    constexpr Executor build() {
        auto order = topological_sort();
        return Executor(std::move(order));
    }

private:
    //! Performs Kahn’s algorithm for topological sort over the internal
    //! scheduler dependency graph of systems. Time complexity is O(V + E),
    //! where V is the number of systems and E is the number of dependency
    //! edges.
    //!
    //! \return Topologically ordered vector of systems for a valid execution.
    //! \throws std::runtime_error
    //!         Thrown if the dependency graph contains a cycle, making a
    //!         topological ordering impossible.
    //! \note The returned pointers are non-owning and remain valid only as long
    //!       as the underlying systems stored in `m_systems` remain alive.
    //! \note The relative order of systems with no dependencies between them
    //!       is unspecified.
    [[nodiscard]]
    constexpr std::vector<System*> topological_sort() {
        // TODO: Assert if there's node for every referenced node in the edges
        // container.

        std::unordered_map<std::type_index, std::size_t> incomming_edges_count;

        // Initialize incomming_edges_count
        for (auto& [node, _] : m_edges) {
            // Possible allocation
            incomming_edges_count[node] = 0;
        }

        // Count incomming edges
        for (auto& [from, tos] : m_edges) {
            // The std::type_index is raw ptr wrapper to type_info therefore
            // we copy instead of reference.
            for (auto to : tos) {
                incomming_edges_count[to] += 1;
            }
        }

        std::queue<std::type_index> ready_nodes;
        for (auto& [node, count] : incomming_edges_count) {
            if (count == 0) {
                ready_nodes.push(node);
            }
        }

        std::vector<System*> result;
        while (!ready_nodes.empty()) {
            auto node = ready_nodes.front();
            ready_nodes.pop();
            result.push_back(m_systems.at(node).get());

            for (auto& to : m_edges[node]) {
                if (--incomming_edges_count[to] == 0) {
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
    std::unordered_map<std::type_index, std::unique_ptr<System>> m_systems;
    std::unordered_map<std::type_index, std::vector<std::type_index>> m_edges;
};

} // namespace kzn