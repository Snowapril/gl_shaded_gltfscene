#ifndef BOUDNING_BOX_HPP
#define BOUDNING_BOX_HPP

#include <glm/vec3.hpp>

namespace GL3 {

	class BoundingBox
	{
	public:
		//! Default constructor
		BoundingBox();
		//! Default destructor
		~BoundingBox();
		//! Default copy constructor
		BoundingBox(const BoundingBox& bb);
		//! Default copy assign operator
		BoundingBox& operator=(const BoundingBox& bb);
		//! Merge the point
		void Merge(const glm::vec3 newPoint);
		//! Merge the bounding box
		void Merge(const BoundingBox& bb);
		//! Reset the bounding box
		void Reset();
		//! Corner getter
		inline glm::vec3 GetLowerCorner() const
		{
			return _lowerCorner;
		}
		inline glm::vec3 GetUpperCorner() const
		{
			return _upperCorner;
		}
	private:
		glm::vec3 _lowerCorner;
		glm::vec3 _upperCorner;
		bool _bFirstMerge;
	};

};

#endif //! end of BoundingBox.hpp