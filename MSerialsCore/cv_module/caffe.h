/*
using opencv lib to load caffe model, ie not using caffe

usage:
1.to load params
cv_load_vgg_params(char *model_txt, char *train_file, char* words)

2.to predict
cv_vgg_predict(const cv::Mat &img, int index, float threshold_val)
*/

#pragma once 
#include <opencv2/highgui/highgui.hpp>   
#include <opencv2/imgproc/imgproc.hpp>   
#include <opencv2/core/core.hpp>   
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>




class caffe_predict
{
	
#if 0
	#ifndef RESIZE_SIZE
	#define RESIZE_SIZE cv::Size(35,35)
	#endif
#endif

private:
	cv::dnn::Net net;
	std::vector<cv::String> classNames;
	cv::Size RESIZE_SIZE;
public:
	caffe_predict(char *model_txt, char *train_file, char* words, cv::Size RESIZE = cv::Size(35,35)) { 
		cv_load_vgg_params(model_txt, train_file, words);
		RESIZE_SIZE = RESIZE;
	};
	static cv::Size get_size() {
		return cv::Size(35, 35);
	}

	std::vector<cv::String> cv_load_class_names(const char *filename)
	{
		std::vector<cv::String> classNames;
		std::ifstream fp(filename);
		if (!fp.is_open())
		{
			std::cerr << "File with classes labels not found: " << filename << std::endl;
			exit(-1);
		}

		std::string name;
		while (!fp.eof())
		{
			std::getline(fp, name);
			if (name.length())
				classNames.push_back(name.substr(name.find(' ') + 1));
		}

		fp.close();
		return classNames;
	}

	static void getMaxClass(const cv::Mat &probBlob, int *classId, double *classProb)
	{
		using namespace cv;
		Mat probMat = probBlob.reshape(1, 1); //reshape the blob to 1x1000 matrix
		Point classNumber;
		minMaxLoc(probMat, NULL, classProb, NULL, &classNumber);
		*classId = classNumber.x;
	}

	bool cv_load_vgg_params(char *model_txt, char *train_file, char* words)
	{
		using namespace cv;
		using namespace cv::dnn;
		try {
			//! [Read and initialize network]
			net = cv::dnn::readNetFromCaffe(model_txt, train_file);
			//! [Read and initialize network]
			classNames = cv_load_class_names(words);
#ifdef __AFX_H__ 
			//AfxMessageBox(L"ng");
#endif
			return true;
		}
		catch (cv::Exception& e) {
			std::cerr << "Exception: " << e.what() << std::endl;
			//! [Check that network was read successfully]
			//if (net.empty())
			//{
			std::cout << "can not load file" << std::endl;
			return false;
			//}
			
			//! [Check that network was read successfully]
		}
	}

	std::pair<std::string, float> cv_vgg_predict(const cv::Mat &img, float threshold_val = 0.5)
	{
		if (img.empty())
		{
			std::pair<std::string, float> a("no image", static_cast<float>(0.0));
			return a;
		}
		using namespace cv;
		using namespace cv::dnn;
		using namespace std;
		static std::mutex mtx;
		std::lock_guard<std::mutex> lck(mtx);
		cv::Mat prob;
		try {
			cv::Mat inputBlob = blobFromImage(img, 1.0f, RESIZE_SIZE, cv::Scalar(0, 0, 0), false);
			net.setInput(inputBlob, "data");
			prob = net.forward("prob");
		}
		catch (cv::Exception e)
		{
			std::cout << e.what() << std::endl;
			return std::pair<std::string, float>("Label_Net_Parameter_Error", static_cast<float>(0.0));

		}
		int classId;
		double classProb;
		getMaxClass(prob, &classId, &classProb);//find the best class
		try
		{
			return std::pair<std::string, float>(classNames.at(classId), static_cast<float>(classProb));
		}
		catch (std::out_of_range e)
		{
			return std::pair<std::string, float>("Label_file_Error", static_cast<float>(0.0));
		}
	}

};