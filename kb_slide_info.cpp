#include "kb_slide_info.h"
#include "kb_xml_io.h"

void kb::slide_info::pixel2micrometer(int x1, int y1, double& x2, double& y2)
{
	x2 = ((double)x1 - (double)m_sz[0] * 0.5) * m_mpp[0]  + m_offset[0];
	y2 = ((double)y1 - (double)m_sz[1] * 0.5) * m_mpp[1]  + m_offset[1];

}

void kb::slide_info::micrometer2pixel(double x1, double y1, int& x2, int& y2)
{
	x2 = (int)((x1 - m_offset[0])  / m_mpp[0] + (double)m_sz[0] * 0.5);
	y2 = (int)((y1 - m_offset[1])  / m_mpp[1] + (double)m_sz[1] * 0.5);
}

int kb::slide_info::load(std::string& path_xml)
{
	kb::read_space_xml(path_xml, "slide_info", "mpp", m_mpp, 2);
	kb::read_space_xml(path_xml, "slide_info", "offset", m_offset, 2);
	kb::read_xml(path_xml, "slide_info", "mag", m_mag);
	kb::read_xml(path_xml, "slide_info", "unit", m_unit);
	kb::read_space_xml(path_xml, "slide_info", "size", m_sz, 2);
	kb::read_xml(path_xml, "slide_info", "level_count", m_level_count);
	kb::read_xml(path_xml, "slide_info", "vendor", m_vendor);
	return 0;
}

int kb::slide_info::save(std::string& path_xml)
{
	kb::write_space_xml(path_xml, "slide_info", "mpp", m_mpp, 2);
	kb::write_space_xml(path_xml, "slide_info", "offset", m_offset, 2);
	kb::write_xml(path_xml, "slide_info", "mag", m_mag);
	kb::write_xml(path_xml, "slide_info", "unit", m_unit);
	kb::write_space_xml(path_xml, "slide_info", "size", m_sz, 2);
	kb::write_xml(path_xml, "slide_info", "level_count", m_level_count);
	kb::write_xml(path_xml, "slide_info", "vendor", m_vendor);

	return 0;
}

void kb::slide_info::print()
{
	std::cout << "mpp: " << m_mpp[0] << " " << m_mpp[1] << std::endl;
	std::cout << "offset: " << m_offset[0] << " " << m_offset[1] << std::endl;
	std::cout << "mag: " << m_mag << std::endl;
	std::cout << "unit: " << m_unit << std::endl;
	std::cout << "size: " << m_sz[0] << " " << m_sz[1] << std::endl;
	std::cout << "level_count: " << m_level_count << std::endl;
	std::cout << "vendor: " << m_vendor << std::endl;
}
