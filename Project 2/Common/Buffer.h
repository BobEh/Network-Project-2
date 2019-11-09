#pragma once

#include <string>
#include <vector>

class Buffer
{
public:
	Buffer(size_t size);

	void writeToBuffer32(uint32_t input);
	void writeToBufferAtIndex32(uint32_t input, uint8_t index);
	void writeStringToBuffer(std::string input);
	void writeStringToBufferAtIndes(std::string input, uint8_t index);
	uint32_t readFromBuffer32();
	uint32_t readFromBufferAtIndex32(uint8_t index);
	void writeToBuffer16(uint16_t input);
	void writeToBufferAtIndex16(uint16_t input, uint8_t index);
	uint16_t readFromBuffer16();
	uint16_t readFromBufferAtIndex16(uint8_t index);
	std::string readStringFromBuffer(size_t length);
	std::string readStringAtIndex(size_t length, uint8_t index);
	uint8_t& operator[] (uint8_t i);
	void Clear();

	uint8_t _readIndex;
	uint8_t _writeIndex;
	std::vector<uint8_t> _buffer;
};