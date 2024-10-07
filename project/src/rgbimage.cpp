#include "rgbimage.h"
#include "color.h"
#include "assert.h"
#include <cmath>

RGBImage::RGBImage(unsigned int Width, unsigned int Height) :m_Width(Width), m_Height(Height)
{
	m_Image = new Color[Width * Height];
}

RGBImage::~RGBImage()
{
	delete m_Image;
}

void RGBImage::setPixelColor(unsigned int x, unsigned int y, const Color& c)
{
	if (x >= m_Width|| y>=m_Height)
		return;

		m_Image[(y * m_Width) + x] = c;

}

const Color& RGBImage::getPixelColor(unsigned int x, unsigned int y) const
{
	if (x >= m_Width || y >= m_Height)
		return m_Image[0];

		return m_Image[y * m_Width + x];
}

unsigned int RGBImage::width() const
{
	return m_Width;
}
unsigned int RGBImage::height() const
{
	return m_Height;
}

unsigned char RGBImage::convertColorChannel(float v)
{
	char erg = NULL;
	if (v > 0.0f && v < 1.0f) {
		erg = v * 255;
	}
	else if (v <= 0.0f) {
		erg = 0;
	}
	else if (v >= 1.0f) {
		erg = 255;
	}
	return erg;
}

 RGBImage& RGBImage::SobelFilter(RGBImage& dst, const RGBImage& src, float factor) {
	assert(dst.m_Width == src.m_Width && dst.m_Height == src.m_Width);

	float  m[3][3] = {  {1.0f, 0.0f, -1.0f},
                        {2.0f, 0.0f, -2.0f},
                        {1.0f, 0.0f, -1.0f}     };

	for (int x = 0; x < src.m_Width; x++){
		for (int y = 0; y < src.m_Height; y++) {
			Color colU;
			Color colV;
			for (int j = 0; j <= 2; j++) {
				for (int i = 0; i <= 2; i++) {
					colU += src.getPixelColor(x-i-1,y-j-1) * m[j][i];
					colV += src.getPixelColor(x-i-1,y-j-1) * m[i][j];
				}
			}
			Color col;
			col.R = sqrt(pow(colU.R, 2) + pow(colV.R, 2));
			col.G = sqrt(pow(colU.G, 2) + pow(colV.G, 2));
			col.B = sqrt(pow(colU.B, 2) + pow(colV.B, 2));

			dst.setPixelColor(x, y, col * factor);
		}
	}
	return dst;
}

bool RGBImage::saveToDisk(const char* Filename)
{

    int width = this->m_Width;
    int height = this->m_Height;
    // Berechnung der Größe des Padding für jede Zeile
    int paddingSize = (4 - (width * 3) % 4) % 4;

    // Größe der Bilddaten ohne Header
    int dataSize = (width * 3 + paddingSize) * height;

    // Gesamtgröße der Datei
    int fileSize = 54 + dataSize;

    // Header mit den erforderlichen Informationen
    char header[54] = {
        'B', 'M', // Bitmap-Identifikation
        static_cast<char>(fileSize & 0xff), static_cast<char>(fileSize >> 8 & 0xff), static_cast<char>(fileSize >> 16 & 0xff), static_cast<char>(fileSize >> 24 & 0xff), // Dateigröße
        0, 0, 0, 0, // Reserviert
        54, 0, 0, 0, // Offset der Bilddaten vom Dateianfang
        40, 0, 0, 0, // Größe der Bitmap-Info-Header
        static_cast<char>(width & 0xff), static_cast<char>(width >> 8 & 0xff), static_cast<char>(width >> 16 & 0xff), static_cast<char>(width >> 24 & 0xff), // Breite des Bildes
        static_cast<char>(height & 0xff), static_cast<char>(height >> 8 & 0xff), static_cast<char>(height >> 16 & 0xff), static_cast<char>(height >> 24 & 0xff), // Höhe des Bildes
        1, 0, // Anzahl der Farbebenen
        24, 0 // Farbtiefe in Bits
    };

    // Ausgabedatei öffnen
    FILE* file = fopen(Filename, "wb");
    if (file == NULL) {
        throw std::runtime_error("Fehler beim Öffnen der Datei.");
    }

    // Header in die Datei schreiben
    fwrite(header, 1, 54, file);

    // Bilddaten in die Datei schreiben
    for (int y = height - 1; y >= 0; y--) {
        for (int x = 0; x < width; x++) {
            int i = y * width + x;
            char pixel[3] = { static_cast<char>(convertColorChannel(m_Image[i].B)), static_cast<char>(convertColorChannel(m_Image[i].G)), static_cast<char>(convertColorChannel(m_Image[i].R)) }; // RGB in BGR umkehren, da Bitmap in BGR gespeichert wird
            fwrite(pixel, 1, 3, file);
        }
        char padding[3] = { 0, 0, 0 };
        for (int i = 0; i < paddingSize; i++) {
            fwrite(padding, 1, 1, file);
        }
    }

    // Ausgabedatei schließen
    fclose(file);
    return true;
}
