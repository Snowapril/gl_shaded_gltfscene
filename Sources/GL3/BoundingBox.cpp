#include <GL3/BoundingBox.hpp>
#include <algorithm>

namespace GL3 {

	BoundingBox::BoundingBox()
	{
		Reset();
	}

	BoundingBox::~BoundingBox()
	{
		//! Do nothing
	}
	
	BoundingBox::BoundingBox(const BoundingBox& bb)
	{
		this->_upperCorner = bb._upperCorner;
		this->_lowerCorner = bb._lowerCorner;
		this->_bFirstMerge = bb._bFirstMerge;
	}
	
	BoundingBox& BoundingBox::operator=(const BoundingBox& bb)
	{
		if (this != &bb)
		{
			this->_upperCorner = bb._upperCorner;
			this->_lowerCorner = bb._lowerCorner;
			this->_bFirstMerge = bb._bFirstMerge;
		}

		return *this;
	}

	void BoundingBox::Merge(const glm::vec3 newPoint)
	{
		if (_bFirstMerge)
		{
			this->_lowerCorner = newPoint;
			this->_upperCorner = newPoint;
			_bFirstMerge = false;
		}
		else
		{
			this->_lowerCorner.x = std::min(this->_lowerCorner.x, newPoint.x);
			this->_lowerCorner.y = std::min(this->_lowerCorner.y, newPoint.y);
			this->_lowerCorner.z = std::min(this->_lowerCorner.z, newPoint.z);

			this->_upperCorner.x = std::max(this->_upperCorner.x, newPoint.x);
			this->_upperCorner.y = std::max(this->_upperCorner.y, newPoint.y);
			this->_upperCorner.z = std::max(this->_upperCorner.z, newPoint.z);
		}
	}

	void BoundingBox::Merge(const BoundingBox& bb)
	{
		if (_bFirstMerge)
		{
			*this = bb;
			_bFirstMerge = false;
		}
		else
		{
			this->_lowerCorner.x = std::min(bb._lowerCorner.x, this->_lowerCorner.x);
			this->_lowerCorner.y = std::min(bb._lowerCorner.y, this->_lowerCorner.y);
			this->_lowerCorner.z = std::min(bb._lowerCorner.z, this->_lowerCorner.z);

			this->_upperCorner.x = std::max(bb._upperCorner.x, this->_upperCorner.x);
			this->_upperCorner.y = std::max(bb._upperCorner.y, this->_upperCorner.y);
			this->_upperCorner.z = std::max(bb._upperCorner.z, this->_upperCorner.z);
		}
	}

	void BoundingBox::Reset()
	{
		this->_lowerCorner = glm::vec3(0.0f);
		this->_upperCorner = glm::vec3(0.0f);
		_bFirstMerge = true;
	}
};