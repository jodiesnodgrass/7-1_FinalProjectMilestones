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

