#include "helpers.h"
#include <math.h>
#include <stdbool.h>

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // Calculate average value for each pixel
            int average = round((image[i][j].rgbtRed + image[i][j].rgbtGreen + image[i][j].rgbtBlue) / 3.0);

            // Set each color channel to the average value
            image[i][j].rgbtRed = average;
            image[i][j].rgbtGreen = average;
            image[i][j].rgbtBlue = average;
        }
    }
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width / 2; j++)
        {
            // Swap pixels horizontally
            RGBTRIPLE temp = image[i][j];
            image[i][j] = image[i][width - 1 - j];
            image[i][width - 1 - j] = temp;
        }
    }
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE blurred[height][width];

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int sumRed = 0;
            int sumGreen = 0;
            int sumBlue = 0;
            float count = 0.0;

            // Sum the values of surrounding pixels (including current pixel)
            for (int k = -1; k <= 1; k++)
            {
                for (int l = -1; l <= 1; l++)
                {
                    int row = i + k;
                    int col = j + l;

                    // Check if the neighboring pixel is within the image bounds
                    if (row >= 0 && row < height && col >= 0 && col < width)
                    {
                        sumRed += image[row][col].rgbtRed;
                        sumGreen += image[row][col].rgbtGreen;
                        sumBlue += image[row][col].rgbtBlue;
                        count++;
                    }
                }
            }

            // Calculate average value for each color channel
            blurred[i][j].rgbtRed = round(sumRed / count);
            blurred[i][j].rgbtGreen = round(sumGreen / count);
            blurred[i][j].rgbtBlue = round(sumBlue / count);
        }
    }

    // Copy the blurred image back to the original image
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image[i][j] = blurred[i][j];
        }
    }
}

// Calculate the Sobel operator for an image channel
int calculateSobelOperator(int gx[3][3], int gy[3][3], int row, int col, int height, int width, RGBTRIPLE image[height][width])
{
    int gxRed = 0, gxGreen = 0, gxBlue = 0;
    int gyRed = 0, gyGreen = 0, gyBlue = 0;

    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            int newRow = row + i;
            int newCol = col + j;

            // Check if the neighboring pixel is within the image bounds
            if (newRow >= 0 && newRow < height && newCol >= 0 && newCol < width)
            {
                gxRed += gx[i + 1][j + 1] * image[newRow][newCol].rgbtRed;
                gxGreen += gx[i + 1][j + 1] * image[newRow][newCol].rgbtGreen;
                gxBlue += gx[i + 1][j + 1] * image[newRow][newCol].rgbtBlue;

                gyRed += gy[i + 1][j + 1] * image[newRow][newCol].rgbtRed;
                gyGreen += gy[i + 1][j + 1] * image[newRow][newCol].rgbtGreen;
                gyBlue += gy[i + 1][j + 1] * image[newRow][newCol].rgbtBlue;
            }
        }
    }

    // Calculate the magnitude of the gradient
    int red = round(sqrt(gxRed * gxRed + gyRed * gyRed));
    int green = round(sqrt(gxGreen * gxGreen + gyGreen * gyGreen));
    int blue = round(sqrt(gxBlue * gxBlue + gyBlue * gyBlue));

    // Cap the values at 255
    red = red > 255 ? 255 : red;
    green = green > 255 ? 255 : green;
    blue = blue > 255 ? 255 : blue;

    return red << 16 | green << 8 | blue;
}

// Detect edges in image
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE copy[height][width];

    // Create a copy of the original image
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            copy[i][j] = image[i][j];
        }
    }

    int gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int pixel = calculateSobelOperator(gx, gy, i, j, height, width, copy);

            // Assign the new pixel value to the current pixel in the image
            image[i][j].rgbtRed = (pixel & 0xFF0000) >> 16;
            image[i][j].rgbtGreen = (pixel & 0x00FF00) >> 8;
            image[i][j].rgbtBlue = pixel & 0x0000FF;
        }
    }
}
