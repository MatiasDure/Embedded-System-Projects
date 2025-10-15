#ifndef __RING_BUFFER__
#define __RING_BUFFER__

typedef struct {
	uint8_t length;
	volatile uint8_t * buffer;
	volatile uint8_t readPosition;
	volatile uint8_t writePosition;
} ringBuffer;

static inline void ringBuffer_write(ringBuffer * buffer, uint8_t character) {
	buffer->buffer[buffer->writePosition] = character;
	buffer->writePosition = buffer->writePosition + 1U >= buffer->length ? 0U : buffer->writePosition + 1U;
}

static inline uint8_t ringBuffer_read(ringBuffer * buffer) {
	if(buffer->readPosition == buffer->writePosition) return 0x0;
	
	uint8_t nextChar = buffer->buffer[buffer->readPosition];
	buffer->readPosition = buffer->readPosition < (buffer->length - 1U) ? buffer->readPosition + 1U : 0U;
	
	return nextChar;
}

#endif