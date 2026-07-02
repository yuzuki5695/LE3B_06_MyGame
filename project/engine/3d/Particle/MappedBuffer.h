#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <cstring>
#include <functional>

namespace MyEngine {	
	/// <summary>
	/// D3D12のリソースをマップしてCPUからアクセスできるようにするためのラッパー構造体
	/// </summary>
	template<typename T>
	struct MappedBuffer {
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		T* data = nullptr;

		void Create(ID3D12Device* device, std::function<Microsoft::WRL::ComPtr<ID3D12Resource>(ID3D12Device*, size_t) > createFunc, uint32_t count = 1) {
			resource = createFunc(device, sizeof(T) * count);
			resource->Map(0, nullptr, reinterpret_cast<void**>(&data));
			memset(data, 0, sizeof(T) * count
			);
		}

		// Getter
		ID3D12Resource* Get() const { return resource.Get(); }
		D3D12_GPU_VIRTUAL_ADDRESS	GetGPUVirtualAddress() const {
			return resource->GetGPUVirtualAddress();
		}
		T* GetData() { return data; }
		const T* GetData() const { return data; }
		// 演算子
		T* operator->() { return data; }
		const T* operator->() const { return data; }
		T& operator[](size_t index) { return data[index]; }
		const T& operator[](size_t index) const { return data[index]; }
	};
}