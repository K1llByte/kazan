## Kazan First Prototype Roadmap

- ECS Framework
    - Use entt
    - Entity/Component/System classes
    - Transform2DComponent
        - Vec2 position
        - float rotation
        - Vec2 scale
    - SpriteComponent
    - Singleton Registry class with an entt registry

- Event System
    - First use case is vulkan events (SwapchainResize)

- RenderSystem
    - PipelineManager, RenderPassManager
    - With a SpriteSubSystem that just renders 2d textures
    - Render images for offscreen rendering (editor viewport )

- Engine Modules
    - Attached and initialized to an application
    - Live throughtout the lifetime of the application
    - Can specify hard dependencies on other modules
    - This will provide an internal reference to those hard dependencies

- Editor architecture
    - Editor
        - Responsible for intilializing imgui context
        - Owns panels

## Roadmap Ordered Todo

- Engine Modules
- VulkanContext/GraphicsContext singleton
    - GraphicsContext::instance()
    - GraphicsContext::device()
    - GraphicsContext::swapchain()
    - GraphicsContext::dset_allocator()
    - GraphicsContext::dset_cache()
- First render system
- SystemManager

### Modules example

- Required default ctor
    - After ctor, module state is considered fully initialized


```c++
class GraphicsModule: Module<WindowsModule> {
    GraphicsModule(Window& window);
    ~GraphicsModule() = default;

    vk::Instance&  instance()
    vk::Device&    device()
    vk::Swapchain& swapchain()
};
```

```c++

```

```c++
int main() {
    Module<GraphicsModule>()
}
m_modules.emplace<GraphicsModule>();
```


### Entity usage
```c++
Entity player = Registry::create();
auto& transform2d_comp = player.emplace<Transform2DComponent>(Vec2(1,2));
auto& transform2d_comp = player.get<Transform2DComponent>();
player.remove_component()
Registry::destroy(player)
```

<!--
vk::Queue gfx_queue = graphics_queue()
vk::submit(gfx_queue, wait_semaphore)

vk::immediate_submit(gfx_queue, [](){

})
-->



### Event System
```c++
struct ComponentAdded: Event {};
struct ComponentRemoved: Event {};
struct SwapchainResized: Event {};

using ComponentEvents = MultipleEvents<
    ComponentAdded,
    ComponentRemoved
>;

void on_component_added(ComponentAdded& event);



// - event argument will be a polymorphic type
// We can check the type of event anytime with
// event.type() == EventType<SwapchainResized>() 
// or event.is<SwapchainResized>()
// - event.visit should allow event argument as 
// ref or const ref
void on_event(Event& event) {
    switch(event.id()) {
        case SwapchainResized::id():
        break;
        case SwapchainResized::id():
        break;
        case SwapchainResized::id():
        break;
    }

    event.visit([](SwapchainResized& event) {

    });
    // Or
    event.visit(
        on_component_added,
        [](ComponentRemoved& event) {}
    );
}
```


```c++
// Sending and listening to events 

// EventListener helper class will auto register
// and unregister events in the EventManager once
// ctor and dtor are called

class EventListener {
    // Returns false if event type is already registered in EventManager
    template<typename E> 
    bool listen_event();
    // Returns false if event type is not registered in EventManager
    template<typename E>
    bool unlisten_event();
};

using FooEventListener = EventListener<SwapchainResized>;

class Foo: FooEventListener;

int main() {
    EventManager::send(SwapchainResized{});
}
```


```c++
//! The main singleton manager responsible for forwarding
class EventManager {
    void send(const E&);
    void listen(T*, RegisteredEvent);
};

// Base class of peers that should handle events 
class EventListener {
    bool register_events(Events&&...);
    bool unregister_events(Events&&...);
};
```