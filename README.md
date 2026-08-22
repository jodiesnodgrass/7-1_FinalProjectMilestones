# 7-1_FinalProjectMilestones

<img width="968" height="746" alt="image" src="https://github.com/user-attachments/assets/bddd44bc-e5ca-452e-8537-0f2c82c7fa8f" />


What new design skills has your work on the project helped you to craft?

Working on this project pushed me to think like a 3D designer for the first time. Forcing me to shift from a flat 2D mindset to building within a full three-dimensional space. I learned how to balance visual composition by pairing hard geometric objects with organic forms, using scale and positioning to create natural depth. Most importantly, it completely changed how I look at lighting—I had to learn how use ambient fill, direct light angles, and specular highlights to create a 3D realistic scene.

What design process did you follow for your project work?

I followed an iterative design process that started with breaking down my reference photo to isolate the core objects being the vase, bouquet, table, coaster, and lamp. I modeled the scene using basic geometric primitives and programmatically built the bouquet before applying textures and custom shaders to capture surface details like the hammered vase. Finally, I tuned the multi-light setup and camera controls through trial and error to match the original photo's warm lighting and ensure smooth navigation.

How could tactics from your design approach be applied in future work?

Tactics that could help in future work could be breaking down complex scenes into basic components to keep large projects manageable, whether building a 3D environment or modular software architecture. Programmatically generating variation through loops and arrays applies directly to procedural generation like building dynamic foliage or terrain in future 3D applications. Keeping code clean by isolating functions for transformations and lighting makes it easy to reuse assets and adjust environments in future projects without breaking core logic.

What new development strategies did you use while working on your 3D scene?

I adopted a modular development approach by writing dedicated helper functions like SetTransformations() to handle matrix math and texture bindings, keeping the main render loop clean and easy to maintain. I also used programmatic generation to build the bouquet, using loops and array iteration to position, scale, and rotate individual petals and leaves dynamically rather than sculpting every piece by hand. I separated environmental logic into isolated functions like SetupSceneLights() which allowed me to fine-tune lighting values without touching the core rendering code.

How did iteration factor into your development?

Iteration was the core of my development process since I was building and lighting a 3D scene from scratch for the first time. I constantly ran the program to test small incremental changes. For example getting the lighting to match my reference photo took multiple rounds of adjusting diffuse values, attenuation, and beam angles to make sure the light hit both the wall and the table properly without washing out the vase.

How has your approach to developing code evolved throughout the milestones, which led you to the project’s completion?

My approach evolved from writing linear, hardcoded scripts to building a clean, modular architecture. First I was focused just on getting basic shapes to display on the screen, which was at first not even similar to the real picture vase. As the scene grew more complex by adding textures and adding lighting, I shifted toward writing reusable helper functions to handle transformations and lighting setups separately. By the final project my goal was just to add the rest of the flowers, placing the flowers, and adjusting the lighting and shape of vase to fine tune the scene. My focus had fully transitioned to code maintainability using loops, arrays, and parameterized functions so I could tweak visual elements and lighting parameters quickly without breaking the rest of the application.

How do computational graphics and visualizations give you new knowledge and skills that can be applied in your future educational pathway?

Working with computational graphics and visualizations builds a solid foundation in low-level linear algebra and matrix transformations which directly translates to advanced computer science coursework like computer vision, game engine development, and machine learning. Managing camera view matrices, coordinate spaces, and vertex shaders sharpens your spatial reasoning and data visualization skills, making it easier to represent complex multidimensional data clearly.

How do computational graphics and visualizations give you new knowledge and skills that can be applied in your future professional pathway?

I am not sure what my future professional pathway will lead to. But mastering computational graphics and visualizations builds technical skills that translate directly into software engineering, data science, and user interface development. Translating real-world visual concepts into mathematical models sharpens your problem-solving ability, making it easier to handle complex software architectures and algorithm design.

