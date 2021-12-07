#include <string>
#include <vector>
#include <iostream>

#include "openslide.h"
#include "openslide-features.h"

#include <opencv2/opencv.hpp>

#include "kb_slide_info.h"

//
//	libopenslide.lib
//

#pragma once

namespace kb
{
	//--------------------------------------------------------------------
	//	openslide object
	//--------------------------------------------------------------------
	class COpenslide
	{
	public:
		COpenslide()
		{
			init();
		}

		COpenslide(std::string& path1)
		{
			create(path1);
		}
		virtual ~COpenslide()
		{
			clear();
		}

		int create(std::string& path1);


		inline void clear()
		{
			if (m_handle != NULL) {
				openslide_close(m_handle);
			}

			init();
		}
		inline void init()
		{
			m_handle = NULL;
			m_numThread = 8;
		}

		//	run std::cout about slide properties
		void print_property();

		//	set slide header to member variables.
		void update_property();


		//	‘S‘Ì‰æ‘œŽæ“¾
		int getImage_whole(cv::Mat& mat1, int level1, double& ratio);
		//	
		int getImage(cv::Mat& mat1, int64_t x1, int64_t y1, int64_t w, int64_t h, int level);



	public:
		openslide_t* m_handle;

		//	slide infomation object
		slide_info m_info;

		//	width and height in each level
		//	buffer size: m_num_level*2
		std::vector<int64_t> m_v_sz;

		//	downsampling
		//	buffer size: m_num_level
		std::vector<double> m_v_ds;


		//
		int m_numThread;
		//
		static int m_debug;

	};



};
