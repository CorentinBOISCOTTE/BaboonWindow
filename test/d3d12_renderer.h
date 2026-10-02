#ifndef __D3D12_RENDERER_H__
#define __D3D12_RENDERER_H__

#include "renderer.h"

#define INITGUID
#include <initguid.h>
#include <d3d12.h>
#include <dxgi1_6.h>

typedef struct D3D12Renderer
{
    ID3D12Device* device;
    IDXGISwapChain4* swap_chain;
    ID3D12CommandQueue* command_queue;
    ID3D12CommandAllocator* command_allocator;
    ID3D12GraphicsCommandList* command_list;
    ID3D12DescriptorHeap* rtv_heap;
    ID3D12Resource* render_targets[2];
    UINT rtv_descriptor_size;
    UINT current_frame;
} D3D12Renderer;

#endif
