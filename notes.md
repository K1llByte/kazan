# Todo

- [x] Add versioned cmake dependencies
- [x] Refactor CMakeLists.txt to allow adding kazan/paperlib as a submodule and write a simple application using engine
- [x] Make BitMayhem compile again
- [-] Disable warnings for external libs
- [x] Unify paper and kazan into same lib
- [x] BasicApp base class
- [x] Test example that just displays a texture
- [x] Change console colors
- [x] Simple system scheduler (DAG + topologycal sort)
    - [ ] Express dependencies using associated type `using Before = TypeList<RenderSystem>`
- [ ] Make EditorSystem only initialize after RenderSystem, since it depends on it and the renderer
- [ ] Refactor Renderer into a non singleton
- [ ] Refactor Registry into a non singleton
- [ ] Remove Singleton base class
- [ ] Reuse simple imgui overlay to display text information such as rendering info
- [ ] Brainstorm how to deal with relative engine/app path
- [ ] Brainstorm a way to costumize render stages in the RenderSystem
- [ ] Write small cli to create application and help
  - `kzn compile shaders`
  - `kzn create example_app`
  - `kzn clean`
  - `kzn build`


- Make SpriteGeometryCache work like a ref counted allocator, when theres no more references left, destroy the geometry
- Current approach for default material is not working, brainstorm a MaterialCache that has a default material.

- Hierarchy component
    - A way to parent entities to each other
    - have a way to get world position of an entity

- Bullets collision 
    - Make bullets collision with player without pussing the player entity
    - Make bullets collision with player kill the player based on weapon pok
    - Add bullet knockback

- Add a shoot function per weapon that spawns necessary bullets

- Fix alpha test not working for alpha textures between players texture (if players are in same depth)

- Specify logs location (Trace,Info,Warning,Error)

- Bug: Physics segfaults if theres a system that runs before it, that initializes a physics component
    - This is a current design flaw of components

- Restructure engine/game folder architecture and cmake build configuration
    - engine/
        - core/
        - animation/
        - ecs/
        - ...
    - game/
        - player_system.hpp
        - game_app.cpp

- Basic Audio system/component  

- Area System (no collision detection, just transform pivot)

- Basic Particle component (specialized game component)
- Script system (basically just have a script component that has a std::function<void(float)> that is invoked)

- Radius area
    - AreaEvent (with entity id)
    - AreaComponent(AreaShape) that sends AreaEvent
    - Gameplay systems should listen to this event

- Scene abstraction

- Figure out how to submit mat3 uniform data (alignment not correct)

- Brainstorm reloadable resources and ways of triggering callbacks when a
resource is reloaded
    - Maybe something consistent with current EventManager implementation

- Rendering improvements
    - Multisample anti-aliasing
    - Mipmaps
    - Anisotropic filtering
    - Improve vk::Image to be more generic

- Pipeline layout improvements
    - Complete PipelineConfig methods
    - Shader reflection to derive pipeline configuration

- In engine shader compilation

- Rethink module + entity system architecture to have possibility to access 
shared data such as `console`, `registry`
    - Current problem: How can I specify root window init params for the module
    if its supposed to be default initializable? 

- Setup engine tracing and profiler with Superluminal

- Make EventHandlerId a mix of the event type id + counter 

# Done

- [Done] Make SpriteMaterial instance
    - [Done] Material should have id for fast comparison
    - [Done] Material should specify albedo, offset, size
- [Done] Defer handling of graphics objects to the RenderSystem
- [Done] Add a SpriteGeometryCache to avoid uploading multiple simple square vertices to the gpu (it stalls the gpu)
- [Done] Make player shoot in the direction he's in
- [Done] Make camera movement take into account multiple players
    - [Done] Make the camera follow the average position of all active players
- [Done] Add PlayerControls to PlayerComponent to allow player to costumize input controls
- [Done] Fix camera view matrix in shader, position not working properly
- [Done] Improve BasicHashedString utility type
    - using HashedString = BasicHashedString<std::uint32_t, char>
    - ctor for `const value_type (&str)[N]`
    - ctor for `const value_type *str, const size_type len`
    - This HashedString will be smaller than entt::hashed_string because it
    wont store the string pointer and size
- [Done] Organize RenderSystem into stage classes (SpriteStage, DebugStage, EditorStage)
    - For now, stage classes will be hardcoded in RenderSystem, but in the future there ill be a way to specify a render graph of these stages
    - ...Stage::pre_render
    - ...Stage::render
- [Done] ConsolePanel:
    - [Done] Auto-complete
    - [Done] History
- [Done] Console: Commands with arguments
    - [Done] ConsoleType concept
- [Done] MemLeak: Some depth image allocation is leaking at end of program
- [Done] Fix depth sorting rendering
- [Done] Update player system to use physics system
- [Done] Physics:
    - Collision detection
    - Rigidbody dynamics
    - For now everything is in a PhysicsComponent since I cant have
    functionality for non colliding dynamic bodies with just RigidbodyComponent
- [Done] Deletion queue not destroying vk objects when windows doesnt resize
- [Done] Debug render pipeline (DebugPipeline) always overlap
    - [Done] Rect
    - [Done] Line
- [Done] Make CameraComponent rendering use camera transform component
- [Done] Make CameraComponent rendering use zoom parameter
- [Done] Texture with parameter texture path
- [Done] Context avaiable to systems
    - auto& window = context<Window>();
    - auto& console = context<Console>();
- [Done] Input: 
    - scroll_callback
    - key_callback
    - cursor_position_callback
    - mouse_button_callback
    - gamepad_connected_callback
- [Done] Console
    - This is needed to avoid having to recompile everytime I want to test
    small change
    - Handy for editing and prototyping in realtime
    - [10/3] Basic Commands with no arguments
    - Commands with arguments
    - Variables
    // bool create_variable();
    // bool delete_variable();
    // void assign_variable();
- [Done] ImGui overlay and editor system
- [Done] Brainstorm input handling and mapping
    - [Done] Keyboard
    - [Done] Mouse
    - [Done] Gamepads
- [Done] Sprite animator
- [Done] Basic sprite rendering
    - [25/2] VertexBuffer and IndexBuffer implementation
    - [25/2] Basic sprite component
    - [Done] Textures
    - [Done] Alpha blending
    - [Done] Texture slices
- [Done] Resource Manager
    - This is needed to avoid loading same resource multiple times
    - Maps resource file paths to ref counted resource data
    - Each resource specifies a way to load that resource, the only thing the
    user needs to do is do `auto texture = resources.load<Texture>("assets/...");`
- [3/3] Implement Camera2DComponent (push constants only for now)
- [Done] Implement Transform2DComponent
- [Done] Implement KZN_ASSERT with possibility of disabling asserts with
KZN_DISABLE_ASSERT


___

# Notes

## Compiling 

#### Generate command:  
`cmake -Bbuild` (optionally specify the generator `-G Ninja`)  
#### Build command:  
`cmake --build build --config Release --target Game`  
#### Generate and compile everything:  
`cmake -Bbuild && cmake --build build`  

## App
```c++
// Let App have a prottected default ctor and a ctor for costumizing common
// things like window.
class EditorApp : public App {
public:
    // Use explicit App ctor
    EditorApp()
        : App("Editor", AppOptions{ 
            .width = 800,
            .height = 600,
        }) {}

    // Or use App implicit default ctor (will be call "App") 
    EditorApp() {}
};
```

## Event system

```c++
// Register handlers
auto handler_ptr = EventManager::listen([](const ResizeEvent&) {});
const auto handler_id = ;
EventManager::unlisten(handler_ptr);
```


```c++
// Create an interface EventListener to allow a class to automatically
// unlisten to their events when destructor is called
listen(this, &ScreenPass::on_swapchain_resize)
listen(&on_swapchain_resize);

// Improve event manager api for shorter listener registration
EventManager::listen(this, &ScreenPass::on_swapchain_resize);
EventManager::listen(&on_swapchain_resize);
```


## Module management

Modules are a way to enable engine functionality and manage globally available instance data
Modules will be in the core layer of the engine and loaded by the App.

Since there are modules that will require to use other module's data/api.
There will be a way to describe inter module dependencies, they can be hard or soft dependencies.  

**Hard dependencies** will be modules that the current module will require in order to be able to function properly. Usage of data instance of required modules is garanteed to be present no need to check before use.

**Soft dependencies** will be optionally specified module dependencies that will not be required for the module to work but will enable extra features for that module. Usage of data instance of optional modules needs to be checked if its loaded before use.

> **Note:** Modules scope should be small, although there will be big modules such as graphics that will do a lot and manage a lot of data.  
> Should be default constructible


Examples of common modules:  
- WindowModule
- InputModule (depends on WindowModule)
- GraphicsModule (depends on WindowModule)
- PhysicsModule
- AudioModule
- EcsModule (should this be a module? or part of core?)

For now, modules will not be implemented using DLLs since I have no intenton reusing the code.

<!-- Iteration 1 -->

```c++
// Module implementation
class GraphicsModule: public Module {
    using Required = TypeList<EcsModule>;
    using Optional = TypeList<PhysicsModule>;
    
    // Ctor is the loading of the modules, requied modules should already have been loaded.
    GraphicsModule(ModuleContext&& context)
        : Module(std::move(context)) {
        // Initialize renderer data
        Renderer::create();
        
        // Register Ecs system
        m_systems.emplace<RenderSystem>(...);
    }
};

using DefaultModules = TypeList<
    GraphicsModule,
    PhysicsModule
>;

class ExampleApp: public App {
    GameApp() 
    : m_ecs_mod{}
    , m_window_mod{}
    , m_input_mod{m_window_mod}
    , m_graphics_mod{m_window_mod} {}

    EcsModule m_ecs_mod;
    WindowModule m_window_mod;
    InputModule m_input_mod;
    GraphicsModule m_graphics_mod;
}

// Before starting the app, it will check dependencies between specified modules and build a dependency graph
```

<!-- Iteration 2 -->

```c++
class GraphicsModule {
public:
    GraphicsModule(Modules)
        : Module(modules) {
            get<EcsModule>().systems()
            systems.emplace<RenderSystem>();
        }
    
    GraphicsModule() = default;
};

using GameModules = entt::type_list<GraphicsModule, WindowModule, EcsModule>;

class GameApp {
    GameApp()
        : m_modules<GameModules>{} {
    }
};
```

## Basic sprite rendering

- Implement VertexBuffer and IndexBuffer
    - Vertex type will be a Vec3 position only
- Change current RenderSystem pipeline to have input vertex
- Change sprite shader to have a input vertex

## Console

```c++
// Console is a registry of variables and commands
auto console = Console();
console.create_variable(...);
console.delete_variable(...);
console.create_command(...);
console.delete_command(...);

// Create/delete variable
console.create_variable("renderer.clear_value", Vec3{0,0,0});
console.delete_command("console.clear");
// Create/delete command
// NOTE: Should also accept lambdas, but should be compile time error if its a
// member function.
console.create_command("console.clear", clear_console_cmd);
console.delete_variable("renderer.clear_value");
// Execute any expression. Could be variable assignment or execute a command
console.execute("console.clear");
```

## Pipeline layout improvements

```c++
// From this
.set_layout(vk::PipelineLayout{
    .push_constants = {vk::push_constant_range<PvmPushData>()},
    .descriptor_sets =
        {Renderer::dset_layout_cache().create_layout({
            vk::uniform_binding(0),
        }),
        Renderer::dset_layout_cache().create_layout({
            vk::uniform_binding(0),
        })}
})

// To this
.set_layout(vk::PipelineLayoutBuilder()
    .add_push_constant<PvmPushData>()
    .add_push_constant<OtherPushData>()
    .add_dset_layout({vk::uniform_binding(0)})
    .add_dset_layout({vk::uniform_binding(0)})
    .build()
)

// FIXME: Possible problem is that adding each call might reallocate the push
// constants layout container. Possible fix would be to have a way to specify
// multiple push constants and dsets at once.
.set_layout(vk::PipelineLayoutBuilder()
    .add_push_constants<PvmPushData, OtherPushData>()
    .add_dset_layouts({vk::uniform_binding(0)}, {vk::uniform_binding(0)})
    .build()
)
```

## Sprite Animation

```c++

enum SkullAnimation {
    WalkingUp,
    WalkingLeft,
    WalkingDown,
    WalkingRight,
};

// Setup animation sheet that describes every animation state with the
// animation sprite indices and animation times.
// NOTE: If any sprite index is out of bounds of the slice setup, or if
// animation state index is not being added in the correct order, add_animation
//  throws InvalidAnimationError.
// This sheet will probably be loaded from a resource in the future
auto anim_sheet_ptr = std::make_shared<AnimationSheet>(sprite_size, Vec2(9,4))
    ->add_animation(SkullAnimation::WalkingUp,    { {0, 0.1f},   {1, 0.1f},  {2, 0.1f},  {3, 0.1f},  {4, 0.1f},  {5, 0.1f},  {6, 0.1f},  {7, 0.1f},  {8, 0.1f} })
    ->add_animation(SkullAnimation::WalkingLeft,  { {9, 0.1f},  {10, 0.1f}, {11, 0.1f}, {12, 0.1f}, {13, 0.1f}, {14, 0.1f}, {15, 0.1f}, {16, 0.1f}, {17, 0.1f} })
    ->add_animation(SkullAnimation::WalkingDown,  { {18, 0.1f}, {19, 0.1f}, {20, 0.1f}, {21, 0.1f}, {22, 0.1f}, {23, 0.1f}, {24, 0.1f}, {25, 0.1f}, {26, 0.1f} })
    ->add_animation(SkullAnimation::WalkingRight, { {27, 0.1f}, {28, 0.1f}, {29, 0.1f}, {30, 0.1f}, {31, 0.1f}, {32, 0.1f}, {33, 0.1f}, {34, 0.1f}, {35, 0.1f} })

    ->set_animations({
        // WalkingUp
        { {0, 0.1f},   {1, 0.1f},  {2, 0.1f},  {3, 0.1f},  {4, 0.1f},  {5, 0.1f},  {6, 0.1f},  {7, 0.1f},  {8, 0.1f} },
        // WalkingLeft
        { {9, 0.1f},  {10, 0.1f}, {11, 0.1f}, {12, 0.1f}, {13, 0.1f}, {14, 0.1f}, {15, 0.1f}, {16, 0.1f}, {17, 0.1f} },
        // WalkingDown
        { {18, 0.1f}, {19, 0.1f}, {20, 0.1f}, {21, 0.1f}, {22, 0.1f}, {23, 0.1f}, {24, 0.1f}, {25, 0.1f}, {26, 0.1f} },
        // WalkingRight
        { {27, 0.1f}, {28, 0.1f}, {29, 0.1f}, {30, 0.1f}, {31, 0.1f}, {32, 0.1f}, {33, 0.1f}, {34, 0.1f}, {35, 0.1f} },
    })

// AnimationSheet {
//    add_animations()
//    animation(SkullAnimation::WalkingUp) -> TimedSlices
// }

// There will be one animator per SpriteAnimatorComponent
auto animator = SpriteAnimator(anim_sheet_ptr, );
// Set currently active animation, will be processed by the AnimationSystem
animator.set_animation(SkullAnimation::WalkingUp);
// By default set_animation will always loop, if this isnt desired must be explicitly set
animator.set_loop(false);
// If animation ends and is not looped, only last slice will be displayed.
// If we want to fallback to another animation state
animator.set_fallback_animation(SkullAnimation::WalkingUp);

// SpriteAnimator
// - Accumulated time
// - Animation speed
// - Current animation (if current animation ends and is not looped, keep last slice)
// - Loop
// - Fallback animation (this )
```

# Input handling

- Keyboard keys and special keys
- Mouse buttons
- Mouse scrolling
- Gamepads


```c++
class KeyboardState;
class MouseState;
class GamepadState;

auto input = Input(window);
// Returns non owning const reference to keyboard state
keyboard.is_pressed(Key::A);

const auto& mouse = input.mouse();
mouse.delta_position();
mouse.delta_scroll();
mouse.is_pressed(MouseButton::Left);
mouse.is_pressed(MouseButton::Right);
mouse.is_pressed(MouseButton::Middle);
mouse.is_pressed(MouseButton::Button4);
mouse.is_pressed(MouseButton::Button8);

const auto& gamepad = input.gamepad(GamePad1);
gamepad.is_pressed(GamepadButton::Cross);


class Input {
    // Updates state data of input devices (delta values and gamepads state)
    void update_state();

    const Keyboard& keyboard() const;
    const Mouse& mouse() const;
    const Gamepad& gamepad() const;
};

input.keyboard().is_key_down(KeyboardKey::A)
input.keyboard().is_key_up(KeyboardKey::A);

input.mouse().delta_position();
input.mouse().delta_scroll();

input.gamepad(0).is_button_down(GamepadButton::A);

```

# Imgui overlay

```c++
editor.add<ConsolePanel>();
// ...
auto& render_system = m_systems.emplace<RenderSystem>();
render_system.set_editor(editor);
```

# Debug Render

```c++
debug_render.draw_rect(pos_vec2, size_vec2, Vec3{1,1,1});
debug_render.draw_line(pos1_vec2, pos2_vec2, Vec3{1,1,1});
// Internal draw list clears every frame, draw calls need to be resubmited
```

# Hierarchy component

```c++
struct HierarchyComponent {
    Entity parent = Entity::Null;
    std::vector<Entity> children;
};
```