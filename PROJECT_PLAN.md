# SeamCraft Roadmap

## Milestone 1
- [x] Project setup
- [x] SFML window
- [x] Basic application architecture

## Milestone 2
- [x] Milestone 2A: Image infrastructure
- [x] Load PNG, JPG, and JPEG files with SFML
- [x] Store original and current working image
- [x] Display loaded image with aspect-ratio fitting
- [x] Milestone 2B: User-controlled image loading
- [x] Open PNG, JPG, and JPEG images from disk
- [x] Display selected image
- [x] Reset current image back to stored original image
- [x] Show simple image load and error status information

## Milestone 3
- [x] Milestone 3A: Energy calculation
- [x] Compute floating-point energy values for every pixel
- [x] Use grayscale luminance and Sobel gradients
- [x] Store energy map as `std::vector<std::vector<float>>`
- [x] Print temporary energy debug statistics
- [x] Milestone 3B: Energy visualization
- [x] Convert energy map to grayscale image
- [x] Normalize energy values from black to white
- [x] Toggle original image and energy map with the `E` key
- [x] Fit and center energy visualization in the window

## Milestone 4
- [x] Milestone 4A: Pixel graph construction
- [x] Represent every pixel as one graph node
- [x] Use deterministic `nodeId = y * imageWidth + x` indexing
- [x] Connect each pixel to valid down-left, down, and down-right neighbours
- [x] Use destination pixel energy as edge weight
- [x] Print temporary graph debug statistics and sanity checks

## Milestone 5
- [x] Milestone 5A: Dijkstra shortest path
- [x] Compute minimum-cost vertical seam using Dijkstra's algorithm
- [x] Reconstruct shortest path from predecessor chain
- [x] Validate seam structure (length, start row, end row, column adjacency)
- [x] Print temporary seam debug statistics
- [ ] Seam visualization and removal

## Milestone 6
- [ ] Seam highlighting
- [ ] Seam removal animation

## Milestone 7
- [ ] Complete seam carving
- [ ] Resize to target width
- [ ] Save resized image

## Milestone 8
- [ ] UI polish
- [ ] Documentation
- [ ] Final optimization
