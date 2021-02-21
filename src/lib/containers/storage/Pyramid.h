#ifndef COMMON_UTILS_PYRAMID_H
#define COMMON_UTILS_PYRAMID_H

#include <cstdint>
#include <cstdlib>
#include <cmath>

namespace storage {

template<typename T>
class Pyramid {
	friend class TestPyramid;

	T* m_head;
	size_t m_size;
	size_t m_capacity;

public:
	using Height_t = uint8_t;

	Pyramid(T* head, size_t capacity) noexcept
		: m_head(head)
		, m_size(0)
		, m_capacity(capacity) {}

	inline const T* begin() const noexcept {
		return m_head;
	}

	const T* begin(Height_t layer) const noexcept {
		const T* result = nullptr;
		if(layer < height()) {
			result = m_head + layer_offset(layer);
		}
		return result;
	}

	inline const T* end() const noexcept {
		return m_head + m_size;
	}

	const T* end(Height_t layer) const noexcept {
		const T* result = nullptr;
		if(layer < height()) {
			result = m_head + std::min(layer_offset(layer + 1), m_size);
		}
		return result;
	}

	inline size_t size() const noexcept {
		return m_size;
	}

	inline size_t capacity() const noexcept {
		return m_capacity;
	}

	inline Height_t height() const noexcept {
		return std::ceil(std::log2(m_size + 1));
	}

	size_t build(size_t size) noexcept {
		if(size > m_capacity) {
			return 0;
		}
		m_size = size;
		size_t first_leaf_index = first_leaf();
		for(size_t i = 0; i < first_leaf_index; ++i) {
			size_t index = first_leaf_index - i - 1;
			node_down_max(index);
		}
		return m_size;
	}

	void insert(const T& element) noexcept {
		if(m_size < m_capacity) {
			m_head[m_size++] = element;
			leaf_up_max(m_size - 1);
		}
	}

	inline const T* peek() const noexcept {
		return (m_size ? m_head : nullptr);
	}

	inline T* peek() noexcept {
		return (m_size ? m_head : nullptr);
	}

	inline void pop() noexcept {
		if(m_size) {
			*m_head = m_head[m_size - 1];
			m_size--;
			node_down_max(0);
		}
	}

	inline bool pop(T& copy) noexcept {
		if(m_size) {
			copy = *m_head;
			*m_head = m_head[m_size - 1];
			m_size--;
			node_down_max(0);
		}
		return true;
	}

	inline void pop_swap() noexcept {
		if(m_size) {
			std::swap(*m_head, m_head[m_size - 1]);
			m_size--;
			node_down_max(0);
		}
	}


private:

	void node_down_min(size_t index_node) noexcept {
		if(is_node(index_node)) {
			size_t index_left = left(index_node);
			size_t index_right = right(index_node);

			size_t index_min = index_node;
			if(m_head[index_left] < m_head[index_min]) {
				index_min = index_left;
			}
			if(index_right < m_size && m_head[index_right] < m_head[index_min]) {
				index_min = index_right;
			}
			if(index_min != index_node) {
				std::swap(m_head[index_node], m_head[index_min]);
				node_down_min(index_min);
			}
		}
	}

	void node_down_max(size_t index_node) noexcept {
		if(is_node(index_node)) {
			size_t index_left = left(index_node);
			size_t index_right = right(index_node);

			size_t index_max = index_node;
			if(m_head[index_left] > m_head[index_max]) {
				index_max = index_left;
			}
			if(index_right < m_size && m_head[index_right] > m_head[index_max]) {
				index_max = index_right;
			}
			if(index_max != index_node) {
				std::swap(m_head[index_node], m_head[index_max]);
				node_down_max(index_max);
			}
		}
	}

	void leaf_up_min(size_t index_leaf) noexcept {
		if(index_leaf > 0) {
			size_t index_parent = parent(index_leaf);
			size_t index_left = left(index_parent);
			size_t index_right = right(index_parent);

			size_t index_min = index_parent;
			if(m_head[index_left] < m_head[index_min]) {
				index_min = index_left;
			}
			if(index_right < m_size && m_head[index_right] < m_head[index_min]) {
				index_min = index_right;
			}
			if(index_min != index_parent) {
				std::swap(m_head[index_parent], m_head[index_min]);
				leaf_up_min(index_parent);
			}
		}
	}

	void leaf_up_max(size_t index_leaf) noexcept {
		if(index_leaf > 0) {
			size_t index_parent = parent(index_leaf);
			size_t index_left = left(index_parent);
			size_t index_right = right(index_parent);

			size_t index_max = index_parent;
			if(m_head[index_left] > m_head[index_max]) {
				index_max = index_left;
			}
			if(index_right < m_size && m_head[index_right] > m_head[index_max]) {
				index_max = index_right;
			}
			if(index_max != index_parent) {
				std::swap(m_head[index_parent], m_head[index_max]);
				leaf_up_max(index_parent);
			}
		}
	}

	inline size_t leaves() const noexcept {
		return m_size - first_leaf();
	}

	inline size_t first_leaf() const noexcept {
		return (m_size >> 1);
	}

	inline bool is_node(size_t index) noexcept {
		return index < first_leaf();
	}

	// common indexing

	static inline size_t left(size_t index) noexcept {
		return (index << 1) | size_t(1);
	}

	static inline size_t right(size_t index) noexcept {
		return (index + size_t(1)) << 1;
	}

	static inline size_t parent(size_t index) noexcept {
		return (index - size_t(1)) >> 1;
	}

	static inline size_t layer_offset(Height_t layer) noexcept {
		size_t result = 0;
		if(layer < sizeof(size_t) * 8) {
			result = size_t(1) << size_t(layer);
			result--;
		}
		return result;
	}

};

}; // namespace storage

#endif //COMMON_UTILS_PYRAMID_H
