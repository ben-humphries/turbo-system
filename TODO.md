# Large-scale things

- **DONE**: Reorganize systems into appropriate namespaces
- **TODO**: Set up interning system in Rendering so that resources
  don't get loaded more than once. Entities then will have handles
  into the resource lists rather than the direct resources
  themselves. Renderer::render will take these handles as well.
