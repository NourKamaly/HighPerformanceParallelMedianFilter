# HighPerformanceParallelMedianFilter
Noise reduction is a typical pre-processing step to improve the results of later processing (for example, edge detection on an image). Median filtering is very widely used in digital image processing because, under certain conditions, it preserves edges while removing noise.
There is no best median filter size, it's a trial and error process and dependant on the picture.
Using the median filter doesn't guarantee that all salt and pepper noise will be gone.

This MPI solution is implemented using a variable filter size, variable image size,variable number of cores, and processes in much less time that the sequential solution.

# Original unfiltered image:

![girl_in_hat](https://user-images.githubusercontent.com/76780379/177437028-321eb44a-5d74-4643-8c81-4b6ccf698bb8.png)

# Filtered image using the sequential solution:

![outputRes0](https://user-images.githubusercontent.com/76780379/177437125-1ac4b05b-dc9e-4af4-be0c-20249e331a74.png)

# Filtered image using the MPI solution:

![outputRes1](https://user-images.githubusercontent.com/76780379/177437169-ae7619d4-4c80-4ec4-972d-575171441560.png)
