#include "renderer/apis/Vulkan/MemBuffer.h"

#include <cstring>
#include <utils/asserts.h>

using namespace Engine::Renderers::Vulkan;

/**
 * Returns the minimum instance size required to be compatible with devices minOffsetAlignment
 *
 * @param instanceSize The size of an instance
 * @param minOffsetAlignment The minimum required alignment, in bytes, for the offset member (eg
 * minUniformBufferOffsetAlignment)
 *
 * @return VkResult of the buffer mapping call
 */
VkDeviceSize MemBuffer::GetAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) {
  if (minOffsetAlignment > 0) {
    return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
  }
  return instanceSize;
}

MemBuffer::MemBuffer(
  Device& device,
  VkDeviceSize instanceSize,
  uint32_t instanceCount,
  VkBufferUsageFlags usageFlags,
  VkMemoryPropertyFlags memoryPropertyFlags,
  VkDeviceSize minOffsetAlignment,
  bool bindOnCreation
) :
  device{ device }, instanceCount{ instanceCount }, instanceSize{ instanceSize },
  usageFlags{ usageFlags }, memoryPropertyFlags{ memoryPropertyFlags } {
  this->alignmentSize = GetAlignment(instanceSize, minOffsetAlignment);
  this->size = alignmentSize * instanceCount;
  device.createBuffer(this->size, usageFlags, memoryPropertyFlags, buffer, memory, bindOnCreation);
}

MemBuffer::~MemBuffer() {
  this->unmap();
  if (this->buffer)
    vkDestroyBuffer(this->device, this->buffer, this->device.getAllocator());
  if (this->memory)
    vkFreeMemory(this->device, this->memory, this->device.getAllocator());
}

/**
 * Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
 *
 * @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete
 * buffer range.
 * @param offset (Optional) Byte offset from beginning
 *
 * @return VkResult of the buffer mapping call
 */
VkResult MemBuffer::map(VkDeviceSize size, VkDeviceSize offset) {
  ASSERT(this->buffer && this->memory, "Called map on buffer before create");
  return vkMapMemory(this->device, this->memory, offset, size, 0, &this->mapped);
}

/**
 * Unmap a mapped memory range
 *
 * @note Does not return a result as vkUnmapMemory can't fail
 */
void MemBuffer::unmap() {
  if (this->mapped) {
    vkUnmapMemory(this->device, this->memory);
    this->mapped = nullptr;
  }
}

void MemBuffer::bind() {
  VK_CHECK(vkBindBufferMemory(this->device, this->buffer, this->memory, 0));
}

/**
 * Copies the specified data to the mapped buffer. Default value writes whole buffer range
 *
 * @param data Pointer to the data to copy
 * @param size (Optional) Size of the data to copy. Pass VK_WHOLE_SIZE to flush the complete buffer
 * range.
 * @param offset (Optional) Byte offset from beginning of mapped region
 *
 */
void MemBuffer::writeTo(const void* data, VkDeviceSize size, VkDeviceSize offset) {
  ASSERT(this->mapped, "Cannot copy to unmapped buffer");

  if (size == VK_WHOLE_SIZE) {
    std::memmove(this->mapped, data, this->size);
  }
  else {
    char* memOffset = reinterpret_cast<char*>(this->mapped);
    memOffset += offset;
    std::memmove(memOffset, data, size);
  }
}

/**
 * Flush a memory range of the buffer to make it visible to the device
 *
 * @note Only required for non-coherent memory
 *
 * @param size (Optional) Size of the memory range to flush. Pass VK_WHOLE_SIZE to flush the
 * complete buffer range.
 * @param offset (Optional) Byte offset from beginning
 *
 * @return VkResult of the flush call
 */
VkResult MemBuffer::flush(VkDeviceSize size, VkDeviceSize offset) {
  VkMappedMemoryRange mappedRange = {};
  mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
  mappedRange.memory = memory;
  mappedRange.offset = offset;
  mappedRange.size = size;
  return vkFlushMappedMemoryRanges(this->device, 1, &mappedRange);
}

/**
 * Invalidate a memory range of the buffer to make it visible to the host
 *
 * @note Only required for non-coherent memory
 *
 * @param size (Optional) Size of the memory range to invalidate. Pass VK_WHOLE_SIZE to invalidate
 * the complete buffer range.
 * @param offset (Optional) Byte offset from beginning
 *
 * @return VkResult of the invalidate call
 */
VkResult MemBuffer::invalidate(VkDeviceSize size, VkDeviceSize offset) {
  VkMappedMemoryRange mappedRange = {};
  mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
  mappedRange.memory = memory;
  mappedRange.offset = offset;
  mappedRange.size = size;
  return vkInvalidateMappedMemoryRanges(this->device, 1, &mappedRange);
}

/**
 * Create a buffer info descriptor
 *
 * @param size (Optional) Size of the memory range of the descriptor
 * @param offset (Optional) Byte offset from beginning
 *
 * @return VkDescriptorBufferInfo of specified offset and range
 */
VkDescriptorBufferInfo MemBuffer::getDescriptorInfo(VkDeviceSize size, VkDeviceSize offset) const {
  return VkDescriptorBufferInfo{
      this->buffer,
      offset,
      size,
  };
}

/**
 * Copies "instanceSize" bytes of data to the mapped buffer at an offset of index * alignmentSize
 *
 * @param data Pointer to the data to copy
 * @param index Used in offset calculation
 *
 */
void MemBuffer::writeToIndex(const void* data, uint32_t index) {
  this->writeTo(data, instanceSize, index * alignmentSize);
}

/**
 *  Flush the memory range at index * alignmentSize of the buffer to make it visible to the device
 *
 * @param index Used in offset calculation
 *
 */
VkResult MemBuffer::flushIndex(uint32_t index) { return this->flush(alignmentSize, index * alignmentSize); }

/**
 * Create a buffer info descriptor
 *
 * @param index Specifies the region given by index * alignmentSize
 *
 * @return VkDescriptorBufferInfo for instance at index
 */
VkDescriptorBufferInfo MemBuffer::descriptorInfoForIndex(uint32_t index) const {
  return this->getDescriptorInfo(alignmentSize, index * alignmentSize);
}

/**
 * Invalidate a memory range of the buffer to make it visible to the host
 *
 * @note Only required for non-coherent memory
 *
 * @param index Specifies the region to invalidate: index * alignmentSize
 *
 * @return VkResult of the invalidate call
 */
VkResult MemBuffer::invalidateIndex(uint32_t index) {
  return this->invalidate(alignmentSize, index * alignmentSize);
}

bool MemBuffer::resize(VkDeviceSize newSize, VkQueue queue, VkCommandPool pool) {
  if (newSize == this->size)
    return false;

  MemBuffer newBuffer(
    this->device,
    this->instanceSize,
    this->instanceCount,
    this->usageFlags,
    this->memoryPropertyFlags,
    this->alignmentSize,
    true
  );

  this->device.copyBuffer(
    static_cast<VkBuffer>(*this),
    static_cast<VkBuffer>(newBuffer),
    this->size
  );
  this->device.waitIdle();

  this->~MemBuffer();
  this->buffer = newBuffer.buffer;
  this->memory = newBuffer.memory;
  this->size = newSize;
  this->alignmentSize = GetAlignment(this->instanceSize, this->alignmentSize);

  newBuffer.buffer = VK_NULL_HANDLE;
  newBuffer.memory = VK_NULL_HANDLE;


  return true;
}