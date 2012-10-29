#ifndef BITMAP_TEXTURE_H
#define BITMAP_TEXTURE_H
#include "utils.h"
struct Image 
{
	unsigned long sizeX;								 	/**< length of the image.*/
	unsigned long sizeY; 									/**< height of image.*/
	
	
	/**The data stored in each pixel
	Inside any bmp image, what is stored is each pixel's 
	color. It is stored in bgr fashion. So What we will do is we will
	allocate data string when we load image and then store in it
	the data we read from the bmp file.  And finally, we will 
	convert the data into rgb from bgr for opengl to read texture
	properly.
	*/
	char *data; 
};
typedef struct Image Image;

///Image Loader. Loads bmp image inside the Image pointer provided
int ImageLoad(char *filename, Image *image) 
{
    FILE *file;
    unsigned long size;                 /**< size of the image in bytes.*/
    unsigned long i;                    /**< standard counter.*/
    unsigned short int planes;          /**< number of planes in image (must be 1) */
    unsigned short int bpp;             /**< number of bits per pixel (must be 24)*/
    char temp;                          /**< temporary color storage for bgr-rgb conversion.*/

    // make sure the file is there.
    if ((file = fopen(filename, "rb"))==NULL)
    {
			printf("File Not Found : %s\n",filename);
			return 0;
    }
    
    /// seek through the bmp header, up to the width/height,
    fseek(file, 18, SEEK_CUR);

    /// read the width,
    if ((i = fread(&image->sizeX, 4, 1, file)) != 1) {
	printf("Error reading width from %s.\n", filename);
	return 0;
    }
//    printf("Width of %s: %lu\n", filename, image->sizeX);
    
    /// read the height ,
    if ((i = fread(&image->sizeY, 4, 1, file)) != 1) {
	printf("Error reading height from %s.\n", filename);
	return 0;
    }
  //  printf("Height of %s: %lu\n", filename, image->sizeY);
    
    /// calculate the size (assuming 24 bits or 3 bytes per pixel).
    size = image->sizeX * image->sizeY * 3;

    /// now read the planes
    if ((fread(&planes, 2, 1, file)) != 1) {
	printf("Error reading planes from %s.\n", filename);
	return 0;
    }
    if (planes != 1) {
	printf("Planes from %s is not 1: %u\n", filename, planes);
	return 0;
    }

    /// and read the bpp.
    if ((i = fread(&bpp, 2, 1, file)) != 1) {
	printf("Error reading bpp from %s.\n", filename);
	return 0;
    }
    if (bpp != 24) {
	printf("Bpp from %s is not 24: %u\n", filename, bpp);
	return 0;
    }
	
    /// seek past the rest of the bitmap header.
    fseek(file, 24, SEEK_CUR);

    /// read the data. 
    image->data = (char *) malloc(size);
    if (image->data == NULL) {
	printf("Error allocating memory for color-corrected image data");
	return 0;	
    }

    if ((i = fread(image->data, size, 1, file)) != 1) {
	printf("Error reading image data from %s.\n", filename);
	return 0;
    }

    for (i=0;i<size;i+=3) { /// reverse all of the colors. (bgr -> rgb)
			///Since this is the storage algorithm in bmp file.
	temp = image->data[i];
	image->data[i] = image->data[i+2];
	image->data[i+2] = temp;
    }
    
    /// and we're done.
    return 1;
}


void LoadGLTextures(char* name, GLuint* user_id) {	
    /// Load texture image
    Image *image1;
    
    /// allocate space for texture
    image1 = (Image *) malloc(sizeof(Image));
    if (image1 == NULL) {
	printf("Error allocating space for image");
	exit(0);
    }

    if (!ImageLoad(name, image1)) {
		exit(1);
    }        
		#ifndef GRAPHICSOFF
    /// Create Texture	using glGenTexture in the index provided.
    glGenTextures(1, user_id );
    glBindTexture(GL_TEXTURE_2D, *user_id);   // 2d texture (x and y size)

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // scale linearly when image smalled than texture

    // 2d texture, level of detail 0 (normal), 3 components (red, green, blue), x size from image, y size from image, 
    // border 0 (normal), rgb color data, unsigned byte data, and finally the data itself.
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->sizeX, image1->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image1->data);
		#endif
};





#endif