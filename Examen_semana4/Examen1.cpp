
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <list>

using namespace std;
using namespace cv;

int get_max_value (int values[], int ofset)
{
  return (int) distance (values, max_element (values, values + ofset));

};
int get_contrast (Mat *imagen)
{
  //Variables para el histograma
  int histSize = 256;
  /// el rango del nivel del gris 0-255
  float range[] = {0, 256};
  const float *histRange = {range};


  //calcular el histograma
  Mat original_hist;
  calcHist (imagen, 1, 0, Mat (), original_hist, 1, &histSize, &histRange, true, false);

  // encontrar valores minimos y maximo para el contraste
  int min, max;
  //recorremos el histograma de menor a mayor buscando el primer nivel de girs distinto de cero

  for (min = 0; original_hist.at<float> (min) == 0; ++min);
  //recorremos el histograma de menor a mayor buscando el primer nivel de girs distinto de cero
  for (max = 255; original_hist.at<float> (max) == 0; --max);
  return max - min;
};
int get_focus (Mat *imagen)
{
  Mat ImagenBordes, contour_hist;

  // opcion 1 Canny
  Canny (*imagen, ImagenBordes, 100, 150, 3);

  //Variables para el histograma BW
  int histSize2 = 256;
  /// el rango del nivel del BW 0-255
  float range2[] = {0, 256};
  const float *histRange2 = {range2};

  calcHist (&ImagenBordes, 1, 0, Mat (), contour_hist, 1, &histSize2, &histRange2, true, false);
  return (int) contour_hist.at<float> (255);
};
int main ()
{
  Mat imagen[6];
  int focus[6];
  int contraste[6];
  for (int i = 0; i < 6; i++)
    {
      //Cargar la imagen
      std::string NombreDeLaImagen = "Imagen" + std::to_string (i + 1) + ".jpg";
      imagen[i] = imread (NombreDeLaImagen, CV_LOAD_IMAGE_GRAYSCALE);
      if (!imagen[i].data)
        {
          cout << "Error al cargar la imagen: " << NombreDeLaImagen << endl;
          exit (1);
        }
      contraste[i] = get_contrast (&imagen[i]);
      focus[i] = get_focus (&imagen[i]);
    }
  int best_contrast = get_max_value (contraste, 6);
  int best_focus = get_max_value (focus, 6);
  cout << "Imagen con mejor contraste: "
       << best_contrast + 1
       << endl;
  cout << "Contraste: "
       << contraste[best_contrast]
       << endl;
  namedWindow ("Imagen Con Mejor Contraste");
  imshow ("Imagen Con Mejor Contraste", imagen[best_contrast]);
  cout << "Puntos de borde: "
       << focus[best_focus]
       << endl;
  cout << "Imagen con mejor enfoque: "
       << best_focus + 1
       << endl;
  namedWindow ("Imagen Con Mejor Enfoque");
  imshow ("Imagen Con Mejor Enfoque", imagen[best_focus]);
  //Esperar a que se pulse una tecla
  waitKey (0);
}

