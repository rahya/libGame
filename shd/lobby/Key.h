#pragma once

#include <iostream>
#include "shd/network/buffer/Input.h"
#include "shd/network/buffer/Output.h"


namespace shd {

	class Key {

		static constexpr const size_t _keyLength = 32;

	public:

		Key() {
			generateKey();
		}

		using key_t = char[_keyLength];
		const char* get() const {
			return _key;
		}

		constexpr const size_t size() const {
			return _keyLength;
		}

		void generateKey() {

			for (size_t i = 0; i < _keyLength - 1; i += 1) {
				_key[i] = rand() % ('z' - '0' + 1) + '0';
			}
			_key[_keyLength - 1] = '\0';
		}

		friend std::ostream& operator<<(std::ostream& iOut, const Key& iKey) {
			iOut << iKey._key;
			return iOut;
		}

		bool operator==(const Key iKey) const {
			return std::string_view{ iKey._key } == _key;
		}

		void serialize(network::buffer::Output& ioBuffer) const {
			ioBuffer.write(_key);
		}

		bool deserialize(network::buffer::Input& ioBuffer) {
			return ioBuffer.read(_key, _keyLength);
		}


	private:
		key_t _key = { 0 };

	};

}