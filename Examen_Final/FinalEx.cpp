#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/ml/ml.hpp>
#define NCLASE1 18
#define NCLASE2 20
#define NCLASE3 14
#define NENTRE (NCLASE1+NCLASE2+NCLASE3)

using namespace cv;
using namespace std;

static Ptr<ml::TrainData>
//Convertir los datos de una matriz al tipo TrainData
prepare_train_data(const Mat& data, const Mat& responses, int ntrain_samples)
{
	Mat sample_idx = Mat::zeros(1, data.rows, CV_8U);
	Mat train_samples = sample_idx.colRange(0, ntrain_samples);
	train_samples.setTo(Scalar::all(1));
	int nvars = data.cols;
	Mat var_type(nvars + 1, 1, CV_8U);
	var_type.setTo(Scalar::all(ml::VAR_ORDERED));
	var_type.at<uchar>(nvars) = ml::VAR_CATEGORICAL;

	return ml::TrainData::create(data, ml::ROW_SAMPLE, responses,
		noArray(), sample_idx, noArray(), var_type);
}

int main()
{
	Mat imagen, imgTemp, imagen_bin, imagen_color;
	vector<vector<Point> > contours;

	int i = 0, clase, k = 0;
	double perimetro, area;
	char *nombre_clases[3] = { "Tornillo", "Tuerca", "Arandela" };

	// 1) entrenamiento
	char *imagenes[3] = { "Clase1.jpg", "Clase2.jpg", "Clase3.jpg" };

	//Creacion del clasificador de Bayes
	Ptr<ml::NormalBayesClassifier> bayes = ml::NormalBayesClassifier::create();

	// creaci�n de imatrices para almacenar la imformaci�n
	Mat train_data(NENTRE, 2, CV_32FC1);
	Mat response_data(NENTRE, 1, CV_32FC1);
	Mat test_data(NENTRE, 2, CV_32FC1);
	Mat	test_data_2(1, 2, CV_32FC1);

	cout << "Ejemplos para el entrenamiento" << endl << endl;

	for (clase = 0; clase < 3; ++clase){
		imagen = imread(imagenes[clase]);
		if (!imagen.data) {
			cout << "Error al cargar la imagen" << endl;
			return 1;
		}
		/*******************************************************************************************************************************************************/
		cvtColor(imagen, imagen, CV_BGR2GRAY);
		namedWindow("Ventana1");
		imshow("Ventana1", imagen);
		//Variables para el histograma
		threshold(imagen, imagen_bin, 50, 255, CV_THRESH_BINARY_INV);
		dilate(imagen_bin, imagen_bin, Mat(), Point(-1, -1), 1);
				

		namedWindow("Ventana", 1);
		imshow("Ventana", imagen_bin);

		findContours(imagen_bin, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

		cout << "El numero total de " << nombre_clases[clase] << "s es " << contours.size() << endl;
		

		for (size_t idx = 0; idx < contours.size(); idx++){
			area = contourArea(contours[idx], false);
			perimetro = arcLength(contours[idx], 1);

			train_data.at<float>(k, 0) = perimetro;
			train_data.at<float>(k, 1) = area;
			response_data.at<float>(k) = clase;
			k++;
					}
		cvWaitKey(0);
	}

	int ntrain_samples = (int)(NENTRE);
	Ptr<ml::TrainData> tdata = prepare_train_data(train_data, response_data, ntrain_samples);
	bayes->train(tdata);

	// 2) prueba
	imagen = imread("Clasif.jpg", 1);
	imagen_bin = Mat(imagen.size(), 8, 1);
	cvtColor(imagen, imagen, CV_BGR2GRAY);
	threshold(imagen, imagen_bin, 50, 255, CV_THRESH_BINARY_INV);
	dilate(imagen_bin, imagen_bin, Mat(), Point(-1, -1), 1);

	imagen_color = Mat(imagen.size(), CV_8UC1, 3);
	cvtColor(imagen, imagen_color, CV_GRAY2BGR);
	imshow("Ventana", imagen_color);

	findContours(imagen_bin, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	cout << endl << "El numero total de piezas a clasificar es: " << contours.size() << endl;

	Mat clasif(contours.size(), 1, CV_32FC1);

	for (size_t idx = 0; idx < contours.size(); idx++){
		area = contourArea(contours[idx], false);
		perimetro = arcLength(contours[idx], 1);
		test_data_2.at<float>(0, 0) = perimetro;
		test_data_2.at<float>(0, 1) = area;

		clasif.at<float>(idx) = bayes->predict(test_data_2);

		cvtColor(imagen, imagen_color, CV_GRAY2BGR);
		Scalar color_rojo(0, 0, 255);
		drawContours(imagen_color, contours, idx, color_rojo, 2);
		imshow("Ventana", imagen_color);
		cout << "La pieza resaltada " << idx << " es un " << nombre_clases[(int)clasif.at<float>(idx)] << endl;
        waitKey(0);
	}

	return 0;
}

