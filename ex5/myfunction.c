// notes for the compiler
#pragma GCC optimize("Ofast,inline")                             // use Ofast, which includes the flags: O3,fast-math,allow-store-data-races,no-protect-parens
#pragma GCC target("avx,avx2,f16c,fma,sse3,ssse3,sse4.1,sse4.2") // use SIMD
#undef _GLIBCXX_DEBUG                                            // disable debug mode

#include <stdbool.h>

// use of macros instead of functions

// debuging note - notice that a and b are evaluated twice here!
#define min(a, b) ((a < b ? a : b))
#define max(a, b) ((a > b ? a : b))

// macro for sum_pixels_by_weight(pixel_sum *sum, pixel p, int weight)
#define sum_pixels_by_weight(sum, p, weight) ( \
    {                                          \
        (sum).red += ((p).red) * (weight);     \
        (sum).green += ((p).green) * (weight); \
        (sum).blue += ((p).blue) * (weight);   \
    })

#define sum_pixels_weight_one(sum, p, weight) ( \
    {                                           \
        (sum).red += ((p).red);                 \
        (sum).green += ((p).green);             \
        (sum).blue += ((p).blue);               \
    })

#define sum_pixels_weight_minus_one(sum, p, weight) ( \
    {                                                 \
        (sum).red -= ((p).red);                       \
        (sum).green -= ((p).green);                   \
        (sum).blue -= ((p).blue);                     \
    })

#define initialize_sum_pixels_weight_minus_one(sum, p, weight) ( \
    {                                                            \
        (sum).red = (-(p).red);                                  \
        (sum).green = (-(p).green);                              \
        (sum).blue = (-(p).blue);                                \
    })

#define initialize_sum_pixels_weight_one(sum, p, weight) ( \
    {                                                      \
        (sum).red = ((p).red);                             \
        (sum).green = ((p).green);                         \
        (sum).blue = ((p).blue);                           \
    })

#define decide_min_max_pixel(l_pixel) (         \
    {                                           \
        if (current_intensity <= min_intensity) \
        {                                       \
            min_intensity = current_intensity;  \
            min_pixel = l_pixel;                \
        }                                       \
        if (current_intensity > max_intensity)  \
        {                                       \
            max_intensity = current_intensity;  \
            max_pixel = l_pixel;                \
        }                                       \
    })

// macro to calculate current intensity
#define intensity(l_pixel) (((int)(l_pixel).red) + ((int)(l_pixel).green) + ((int)(l_pixel).blue));

// macros for charsToPixels() and pixelsToChars() in order to share resources
#define charsToPixels(data, pixels) (memcpy(pixels, data, mn_sizeof))

#define pixelsToChars(pixels, data) (memcpy(data, pixels, mn_sizeof))

typedef struct
{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} pixel;

typedef struct
{
    int red;
    int green;
    int blue;
    // added to have size of struct be a power of 2
    int alpha;
} pixel_sum;

void myfunction(Image *image, char *srcImgpName, char *blurRsltImgName, char *sharpRsltImgName, char *filteredBlurRsltImgName, char *filteredSharpRsltImgName, char flag)
{
    // calculate expressions once
    register int mn_sizeof = n * n * sizeof(pixel);

    pixel *pixelsImg = malloc(mn_sizeof);
    pixel *backupOrg = malloc(mn_sizeof);
    char *data = image->data;

    // use of macro instead of function
    charsToPixels(data, backupOrg);
    // using inline instead of calling copyPixels(pixelsImg, backupOrg);
    memcpy(pixelsImg, backupOrg, mn_sizeof);

    register short i, j;
    // hardcoded kernelSize to be always 3.
    const short lim = n - 1;

    pixel_sum sum;
    pixel temp_pixel;

    register int pixel_index = 0;

    if (flag == '1')
    {
        // blur image
        // apply the kernel over each pixel. no filter, and kernelScale == 9. (the kernel is blurKernel)
        // hardcoded the Kernel matrix
        for (i = 1; i < lim; ++i)
        {
            for (j = 1; j < lim; ++j)
            {
                // Applies kernel for pixel at (i,j)
                // dst[calcIndex(i, j)] = applyKernel(i, j, src, kernel, kernelScale, filter);

                // doing kernal multiplication without a for-loop
                // unrolled loop completely

                // use temp_pixel as a temp pixel storage place
                temp_pixel = backupOrg[pixel_index];
                initialize_sum_pixels_weight_one(sum, temp_pixel, 1);
                temp_pixel = backupOrg[pixel_index + 1];
                sum_pixels_weight_one(sum, temp_pixel, 1);
                temp_pixel = backupOrg[pixel_index + 2];
                sum_pixels_weight_one(sum, temp_pixel, 1);

                pixel_index += n;
                temp_pixel = backupOrg[pixel_index];
                sum_pixels_weight_one(sum, temp_pixel, 1);
                temp_pixel = backupOrg[pixel_index + 1];
                sum_pixels_weight_one(sum, temp_pixel, 1);
                temp_pixel = backupOrg[pixel_index + 2];
                sum_pixels_weight_one(sum, temp_pixel, 1);

                pixel_index += n;
                temp_pixel = backupOrg[pixel_index];
                sum_pixels_weight_one(sum, temp_pixel, 1);
                temp_pixel = backupOrg[pixel_index + 1];
                sum_pixels_weight_one(sum, temp_pixel, 1);
                temp_pixel = backupOrg[pixel_index + 2];
                sum_pixels_weight_one(sum, temp_pixel, 1);

                // assign kernel's result to pixel at [i,j]
                // inline version of assign_sum_to_pixel(pixel *current_pixel, pixel_sum sum, char kernelScale)

                // assign_sum_to_pixel - Truncates pixel's new value to match the range [0,255]

                sum.red /= 9;
                sum.green /= 9;
                sum.blue /= 9;

                // pixel index = n*i + j, without using any extra multiplication :)
                pixel_index -= n - 1;
                // if there was no filter applied, and we are using the blurKernel, sum fiels will be always positive
                // so we don't need the min(max()) part
                pixelsImg[pixel_index].red = (unsigned char)(sum.red);
                pixelsImg[pixel_index].green = (unsigned char)(sum.green);
                pixelsImg[pixel_index].blue = (unsigned char)(sum.blue);
                pixel_index -= n;
            }
            pixel_index += 2;
        }
        pixelsToChars(pixelsImg, data);

        // write result image to file
        writeBMP(image, srcImgpName, blurRsltImgName);

        // sharpen the resulting image

        charsToPixels(data, backupOrg);
        memcpy(pixelsImg, backupOrg, mn_sizeof);

        // apply the kernel over each pixel. no filter, and kernelScale == 1. (the kernel is sharpKernel)
        pixel_index = 0;
        // hardcoded the Kernel matrix
        for (i = 1; i < lim; ++i)
        {
            for (j = 1; j < lim; ++j)
            {
                // Applies kernel for pixel at (i,j)
                // dst[calcIndex(i, j)] = applyKernel(i, j, src, kernel, kernelScale, filter);

                // doing kernal multiplication without a for-loop
                // unrolled loop completely

                // use temp_pixel as a temp pixel storage place
                temp_pixel = backupOrg[pixel_index];
                initialize_sum_pixels_weight_minus_one(sum, temp_pixel, -1);
                temp_pixel = backupOrg[pixel_index + 1];
                sum_pixels_weight_minus_one(sum, temp_pixel, -1);
                temp_pixel = backupOrg[pixel_index + 2];
                sum_pixels_weight_minus_one(sum, temp_pixel, -1);

                pixel_index += n;
                temp_pixel = backupOrg[pixel_index];
                sum_pixels_weight_minus_one(sum, temp_pixel, -1);
                temp_pixel = backupOrg[pixel_index + 1];
                sum_pixels_by_weight(sum, temp_pixel, 9);
                temp_pixel = backupOrg[pixel_index + 2];
                sum_pixels_weight_minus_one(sum, temp_pixel, -1);

                pixel_index += n;
                temp_pixel = backupOrg[pixel_index];
                sum_pixels_weight_minus_one(sum, temp_pixel, -1);
                temp_pixel = backupOrg[pixel_index + 1];
                sum_pixels_weight_minus_one(sum, temp_pixel, -1);
                temp_pixel = backupOrg[pixel_index + 2];
                sum_pixels_weight_minus_one(sum, temp_pixel, -1);
                // assign kernel's result to pixel at [i,j]
                // inline version of assign_sum_to_pixel(pixel *current_pixel, pixel_sum sum, char kernelScale)

                // pixel index = n*i + j, without using any extra multiplication :)
                pixel_index -= n - 1;
                pixelsImg[pixel_index].red = (unsigned char)(min(max(sum.red, 0), 255));
                pixelsImg[pixel_index].green = (unsigned char)(min(max(sum.green, 0), 255));
                pixelsImg[pixel_index].blue = (unsigned char)(min(max(sum.blue, 0), 255));

                pixel_index -= n;
            }
            pixel_index += 2;
        }

        pixelsToChars(pixelsImg, data);

        // write result image to file
        writeBMP(image, srcImgpName, sharpRsltImgName);
    }
    else
    {
        register int min_intensity;
        register int max_intensity;
        register int current_intensity;
        pixel min_pixel, max_pixel;

        // apply extermum filtered kernel to blur image
        // apply the kernel over each pixel. filter == 1 and kernelScale == 7. (the kernel is blurKernel)
        // hardcoded the Kernel matrix
        for (i = 1; i < lim; ++i)
        {
            for (j = 1; j < lim; ++j)
            {
                // Applies kernel for pixel at (i,j)
                // dst[calcIndex(i, j)] = applyKernel(i, j, src, kernel, kernelScale, filter);

                // doing kernal multiplication without a for-loop
                // unrolled loop completely

                // use temp_pixel as a temp pixel storage place
                temp_pixel = backupOrg[pixel_index];
                initialize_sum_pixels_weight_one(sum, temp_pixel, 1);
                min_intensity = max_intensity = intensity(temp_pixel);
                min_pixel = max_pixel = temp_pixel;

                temp_pixel = backupOrg[pixel_index + 1];
                sum_pixels_weight_one(sum, temp_pixel, 1);
                current_intensity = intensity(temp_pixel);
                decide_min_max_pixel(temp_pixel);

                temp_pixel = backupOrg[pixel_index + 2];
                sum_pixels_weight_one(sum, temp_pixel, 1);
                current_intensity = intensity(temp_pixel);
                decide_min_max_pixel(temp_pixel);

                pixel_index += n;
                temp_pixel = backupOrg[pixel_index];
                sum_pixels_weight_one(sum, temp_pixel, 1);
                current_intensity = intensity(temp_pixel);
                decide_min_max_pixel(temp_pixel);

                temp_pixel = backupOrg[pixel_index + 1];
                sum_pixels_weight_one(sum, temp_pixel, 1);
                current_intensity = intensity(temp_pixel);
                decide_min_max_pixel(temp_pixel);

                temp_pixel = backupOrg[pixel_index + 2];
                sum_pixels_weight_one(sum, temp_pixel, 1);
                current_intensity = intensity(temp_pixel);
                decide_min_max_pixel(temp_pixel);

                pixel_index += n;
                temp_pixel = backupOrg[pixel_index];
                sum_pixels_weight_one(sum, temp_pixel, 1);
                current_intensity = intensity(temp_pixel);
                decide_min_max_pixel(temp_pixel);

                temp_pixel = backupOrg[pixel_index + 1];
                sum_pixels_weight_one(sum, temp_pixel, 1);
                current_intensity = intensity(temp_pixel);
                decide_min_max_pixel(temp_pixel);

                temp_pixel = backupOrg[pixel_index + 2];
                sum_pixels_weight_one(sum, temp_pixel, 1);
                current_intensity = intensity(temp_pixel);
                decide_min_max_pixel(temp_pixel);

                // filter out min and max

                // use of macros instead of functions
                sum_pixels_weight_minus_one(sum, min_pixel, -1);

                // use of macros instead of functions
                sum_pixels_weight_minus_one(sum, max_pixel, -1);

                // assign kernel's result to pixel at [i,j]
                // inline version of assign_sum_to_pixel(pixel *current_pixel, pixel_sum sum, char kernelScale)

                // assign_sum_to_pixel - Truncates pixel's new value to match the range [0,255]

                sum.red /= 7;
                sum.green /= 7;
                sum.blue /= 7;

                // pixel index = n*i + j, without using any extra multiplication :)
                pixel_index -= n - 1;
                pixelsImg[pixel_index].red = (unsigned char)(min(max(sum.red, 0), 255));
                pixelsImg[pixel_index].green = (unsigned char)(min(max(sum.green, 0), 255));
                pixelsImg[pixel_index].blue = (unsigned char)(min(max(sum.blue, 0), 255));

                pixel_index -= n;
            }
            pixel_index += 2;
        }
        pixelsToChars(pixelsImg, data);

        // write result image to file
        writeBMP(image, srcImgpName, filteredBlurRsltImgName);

        // sharpen the resulting image

        charsToPixels(data, backupOrg);
        memcpy(pixelsImg, backupOrg, mn_sizeof);
        // apply the kernel over each pixel. no filter, and kernelScale == 1. (the kernel is sharpKernel)
        pixel_index = 0;
        // hardcoded the Kernel matrix
        for (i = 1; i < lim; ++i)
        {
            for (j = 1; j < lim; ++j)
            {
                // Applies kernel for pixel at (i,j)
                // dst[calcIndex(i, j)] = applyKernel(i, j, src, kernel, kernelScale, filter);

                // doing kernal multiplication without a for-loop
                // unrolled loop completely

                // use temp_pixel as a temp pixel storage place
                temp_pixel = backupOrg[pixel_index];
                initialize_sum_pixels_weight_minus_one(sum, temp_pixel, -1);
                temp_pixel = backupOrg[pixel_index + 1];
                sum_pixels_weight_minus_one(sum, temp_pixel, -1);
                temp_pixel = backupOrg[pixel_index + 2];
                sum_pixels_weight_minus_one(sum, temp_pixel, -1);

                pixel_index += n;
                temp_pixel = backupOrg[pixel_index];
                sum_pixels_weight_minus_one(sum, temp_pixel, -1);
                temp_pixel = backupOrg[pixel_index + 1];
                sum_pixels_by_weight(sum, temp_pixel, 9);
                temp_pixel = backupOrg[pixel_index + 2];
                sum_pixels_weight_minus_one(sum, temp_pixel, -1);

                pixel_index += n;
                temp_pixel = backupOrg[pixel_index];
                sum_pixels_weight_minus_one(sum, temp_pixel, -1);
                temp_pixel = backupOrg[pixel_index + 1];
                sum_pixels_weight_minus_one(sum, temp_pixel, -1);
                temp_pixel = backupOrg[pixel_index + 2];
                sum_pixels_weight_minus_one(sum, temp_pixel, -1);
                // assign kernel's result to pixel at [i,j]
                // inline version of assign_sum_to_pixel(pixel *current_pixel, pixel_sum sum, char kernelScale)

                // pixel index = n*i + j, without using any extra multiplication :)
                pixel_index -= n - 1;
                pixelsImg[pixel_index].red = (unsigned char)(min(max(sum.red, 0), 255));
                pixelsImg[pixel_index].green = (unsigned char)(min(max(sum.green, 0), 255));
                pixelsImg[pixel_index].blue = (unsigned char)(min(max(sum.blue, 0), 255));

                pixel_index -= n;
            }
            pixel_index += 2;
        }

        pixelsToChars(pixelsImg, data);

        // write result image to file
        writeBMP(image, srcImgpName, filteredSharpRsltImgName);
    }
    free(pixelsImg);
    free(backupOrg);
}
