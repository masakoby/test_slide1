#include <string>

#pragma once

namespace kb
{
	class slide_info
	{
	public:
		slide_info()
		{
			m_offset[0] = 0.0;
			m_offset[1] = 0.0;
			m_mpp[0] = 1.0;
			m_mpp[1] = 1.0;
			m_mag = 1.0;
			m_sz[0] = 0;
			m_sz[1] = 0;
			m_level_count = 0;
			m_unit = "micrometer";
		}

		//	hamahoto: ***.ndpi
		//	aperio: ***.svs
		//
		//	slide info file: ***_info.xml
		int load(std::string& path_xml);
		int save(std::string& path_xml);

		void pixel2micrometer(int x1, int y1, double& x2, double& y2);
		void micrometer2pixel(double x1, double y1, int& x2, int& y2);

		void print();


	public:
		//	offset
		//		浜ホトは、オフセットはナノメーター単位で記録されている
		//			hamamatsu.XOffsetFromSlideCentre: 8497081
		//			hamamatsu.YOffsetFromSlideCentre : 854012
		//		読み込んだ段階でマイクロメーターにする
		//		
		double m_offset[2];

		//	resolution
		//	デフォルトは、micrometer/pixel
		//		openslide.mpp - x: 0.45273451647953639
		//		openslide.mpp - y : 0.45273451647953639
		double m_mpp[2];

		//	resolution unit
		//		m, cm, mm, micrometer, nanometer
		std::string m_unit;

		//	scanning magnification
		//		openslide.objective - power : 20
		double m_mag;

		//	image width and height
		//	縮小なしの画像サイズ
		//		openslide.level[0].height : 29952
		//		openslide.level[0].width : 20480
		int m_sz[2];

		//	レベル数
		int m_level_count;

		//	vendor
		//		openslide.vendor: hamamatsu
		std::string m_vendor;

	};
};
