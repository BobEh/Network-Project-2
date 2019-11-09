#include "Buffer.h"

Buffer::Buffer(size_t size) : _readIndex(0), _writeIndex(0)
{
	_buffer.resize(size);
}
void Buffer::writeToBuffer32(uint32_t input)
{
	_buffer.at(_writeIndex) = input;
	_buffer.at(_writeIndex + 1) = input >> 8;
	_buffer.at(_writeIndex + 2) = input >> 16;
	_buffer.at(_writeIndex + 3) = input >> 24;

	_writeIndex += 4;
}
void Buffer::writeToBufferAtIndex32(uint32_t input, uint8_t index)
{

	_buffer[index] = input;
	_buffer[index + 1] = input >> 8;
	_buffer[index + 2] = input >> 16;
	_buffer[index + 3] = input >> 24;
}
void Buffer::writeStringToBuffer(std::string input)
{
	memcpy(&(_buffer.at(_writeIndex)), &(input.at(0)), input.length());

	int stringLength = input.length();
	_writeIndex += stringLength;
}
void Buffer::writeStringToBufferAtIndes(std::string input, uint8_t index)
{
	memcpy(&(_buffer.at(index)), &(input.at(0)), input.length());
}
uint32_t Buffer::readFromBuffer32()
{
	uint32_t output = 0;

	output |= _buffer.at(_readIndex);
	output |= _buffer.at(_readIndex + 1) << 8;
	output |= _buffer.at(_readIndex + 2) << 16;
	output |= _buffer.at(_readIndex + 3) << 24;

	_readIndex += 4;

	return output;
}
uint32_t Buffer::readFromBufferAtIndex32(uint8_t index)
{
	uint32_t output = 0;

	output |= _buffer.at(index);
	output |= _buffer.at(index + 1) << 8;
	output |= _buffer.at(index + 2) << 16;
	output |= _buffer.at(index + 3) << 24;

	return output;
}
void Buffer::writeToBuffer16(uint16_t input)
{
	_buffer.at(_writeIndex) = input;
	_buffer.at(_writeIndex + 1) = input >> 8;

	_writeIndex += 2;
}
void Buffer::writeToBufferAtIndex16(uint16_t input, uint8_t index)
{
	_buffer.at(index) = input;
	_buffer.at(index + 1) = input >> 8;
}
uint16_t Buffer::readFromBuffer16()
{
	uint32_t output = 0;

	output |= _buffer.at(_readIndex);
	output |= _buffer.at(_readIndex + 1) << 8;

	_readIndex += 2;

	return output;
}

uint16_t Buffer::readFromBufferAtIndex16(uint8_t index)
{
	uint32_t output = 0;

	output |= _buffer.at(index);
	output |= _buffer.at(index + 1) << 8;

	return output;
}
std::string Buffer::readStringFromBuffer(size_t length)
{
	std::string output(' ', length);
	output.resize(length);

	for (int i = 0; i < length; i++)
	{
		output.at(i) = _buffer.at(_readIndex + i);
	}

	_readIndex += length;
	return output;
}

std::string Buffer::readStringAtIndex(size_t length, uint8_t index)
{
	std::string output(' ', length);
	output.resize(length);

	for (int i = 0; i < length; i++)
	{
		output.at(i) = _buffer.at(index + i);
	}

	return output;
}
uint8_t& Buffer::operator[] (uint8_t i)
{
	return _buffer[i];
}
void Buffer::Clear()
{
	_writeIndex = 0;
	_readIndex = 0;
}