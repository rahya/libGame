#pragma once

#include <cstdint>


namespace game::map {


	struct Size {
		using value_type = uint32_t;
		value_type width;
		value_type height;
		friend bool operator==(const Size& iSize0, const Size& iSize1) { return iSize0.width == iSize1.width && iSize0.height == iSize1.height; }

		void serialize(network::buffer::Output& ioBuffer) const {
			ioBuffer.write(width);
			ioBuffer.write(height);
		}

		void deserialize(network::buffer::Input& ioBuffer) {
			ioBuffer.read(width);
			ioBuffer.read(height);
		}
	};
};