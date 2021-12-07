
#include "kb_openslide.h"



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	kb::COpenslide

int kb::COpenslide::m_debug=0;


int kb::COpenslide::create(std::string& path1)
{
	clear();

	//std::cout << "check 1" << std::endl;

	m_handle = openslide_open(path1.c_str());

	//std::cout << "check 2" << std::endl;

	if (m_handle == NULL) {
		std::cout << "error: openslide_open" << std::endl;
		std::cout << path1 << std::endl;
		return -1;
	}
	else {
		const char* msg = openslide_get_error(m_handle);
		if (msg != NULL) {
			std::cout << msg << std::endl;
		}
	}

	//std::cout << "check 3" << std::endl;

	update_property();


	return 0;
}


void kb::COpenslide::print_property()
{
	if (m_handle == NULL)
		return;


	const char* const* pProp = openslide_get_property_names(m_handle);
	{
		int k = 0;
		while (1) {
			if (pProp[k] == NULL)
				break;
			std::cout << pProp[k] << ": " << openslide_get_property_value(m_handle, pProp[k]) << std::endl;

			k++;
		}
	}
}


//	set slide header to member variables.
void kb::COpenslide::update_property()
{
	if (m_handle == NULL)
		return;

	//
	m_info.m_level_count = openslide_get_level_count(m_handle);

	//
	const char* const* pProp = openslide_get_property_names(m_handle);
	{
		int k = 0;
		while (1) {
			if (pProp[k] == NULL)
				break;
			if (m_debug > 1) {
				std::cout << pProp[k] << ": " << openslide_get_property_value(m_handle, pProp[k]) << std::endl;
			}

			std::string str1 = pProp[k];
			if ("openslide.mpp-x" == str1) {
				//	解像度は、μm/pixel単位で記録されている
				m_info.m_mpp[0] = atof(openslide_get_property_value(m_handle, pProp[k]));
			}
			else if ("openslide.mpp-y" == str1)
			{
				m_info.m_mpp[1] = atof(openslide_get_property_value(m_handle, pProp[k]));
			}
			else if ("hamamatsu.XOffsetFromSlideCentre" == str1) {
				m_info.m_offset[0] = atof(openslide_get_property_value(m_handle, pProp[k])) * 0.001;
			}
			else if ("hamamatsu.YOffsetFromSlideCentre" == str1) {
				m_info.m_offset[1] = atof(openslide_get_property_value(m_handle, pProp[k])) * 0.001;
			}
			else if ("openslide.vendor" == str1){
				m_info.m_vendor = openslide_get_property_value(m_handle, pProp[k]);
			}
			else if ("openslide.objective-power" == str1){
				m_info.m_mag = (double)(atof(openslide_get_property_value(m_handle, pProp[k])));
			}


			k++;
		}
	}

	m_v_sz.clear();
	m_v_ds.clear();
	for (int32_t k = 0; k < m_info.m_level_count; k++) {
		int64_t w = 0, h = 0;
		openslide_get_level_dimensions(m_handle, k, &w, &h);
		double ds = openslide_get_level_downsample(m_handle, k);

		m_v_sz.push_back(w);
		m_v_sz.push_back(h);
		m_v_ds.push_back(ds);

		if (k == 0) {
			m_info.m_sz[0] = w;
			m_info.m_sz[1] = h;
		}

		if (m_debug > 0) {
			std::cout << "level: " << k << ", dimensions: " << w << "," << h << ", downsample: " << ds << std::endl;
		}
	}

	
}


//
int kb::COpenslide::getImage_whole(cv::Mat& mat1, int level1, double& ratio)
{
	if (m_handle == NULL)
		return -1;

	int numThread = m_numThread;
	if (numThread < 1) {
		numThread = 1;
	}
	int32_t level = m_info.m_level_count - 1;
	if (0 <= level1 && level1 < m_info.m_level_count) {
		level = level1;
	}

	int64_t w = m_v_sz[level * 2];
	int64_t h = m_v_sz[level * 2 + 1];

	ratio = 1.0 / m_v_ds[level];
	//std::cout << "size: " <<w<<" "<<h<<", level: "<<m_num_level<< std::endl;

	uint32_t *buf = new uint32_t[w*h];
	openslide_read_region(m_handle, buf, 0, 0, level, w, h);

	cv::Size sz(w, h);
	mat1 = cv::Mat(sz, CV_8UC3);

#pragma omp parallel num_threads(numThread)
	{
#pragma omp for 
		for (int j = 0; j < sz.height; j++) {
			int jw = j * sz.width;
			for (int i = 0; i < sz.width; i++) {
				uint32_t a = buf[i + jw];
				cv::Vec3b a2;
				a2[2] = (a >> 16) & 0xff;
				a2[1] = (a >> 8) & 0xff;
				a2[0] = a & 0xff;

				mat1.at<cv::Vec3b>(j, i) = a2;
			}
		}
	}

	delete[] buf;


	return 0;
}



int kb::COpenslide::getImage(cv::Mat& mat1, int64_t x1, int64_t y1, int64_t w, int64_t h, int level)
{
	if (m_handle == NULL)
		return -1;
	if (w <= 0 || h <= 0)
		return -1;

	int numThread = m_numThread;
	if (numThread < 1) {
		numThread = 1;
	}
	if (level < 0) {
		level = 0;
	}
	else if (m_info.m_level_count <= level) {
		level = m_info.m_level_count - 1;
	}

	//std::cout << "size: " << w << " " << h << ", level: " << m_num_level << std::endl;


	uint32_t *buf = new uint32_t[w*h];

	{
		//std::chrono::system_clock::time_point start = std::chrono::system_clock::now(); // 計測開始時間

		openslide_read_region(m_handle, buf, x1, y1, level, w, h);

		//std::chrono::system_clock::time_point end = std::chrono::system_clock::now();  // 計測終了時間
		//double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count(); //処理に要した時間をミリ秒に変換

		//std::cout << "openslide_read_region: " << elapsed << std::endl;
	}

	cv::Size sz(w, h);
	mat1 = cv::Mat(sz, CV_8UC3);

#pragma omp parallel num_threads(numThread)
	{
#pragma omp for 
		for (int j = 0; j < sz.height; j++) {
			int jw = j * sz.width;
			for (int i = 0; i < sz.width; i++) {
				uint32_t a = buf[i + jw];
				cv::Vec3b a2;
				a2[2] = (a >> 16) & 0xff;
				a2[1] = (a >> 8) & 0xff;
				a2[0] = a & 0xff;

				mat1.at<cv::Vec3b>(j, i) = a2;
			}
		}
	}

	delete[] buf;


	return 0;
}


