1. If a system doesn't need to be instanced, it always gets put in a
   namespace
2. Never use constructors/copy constructors/deconstructors
3. To create an instance, use a static class method (likely entitled
   `create` or `new`)
4. Prefer structs over classes because all members are automatically
   public and private variables are for cowards
5. Any function which doesn't have a specific association to a data
   type or has specific associations to multiple data types should not
   be a member function.
6. Don't use references motherfucker
7. If you make a macro, undefine it when you don't need it anymore
8. When you need to make a TODO comment, the format is as so:
```
// TODO($your_name): $description
```
9. Put spaces before your brackets or you will go to hell
10. Use printf, not cout
