#include "d3d12_renderer.h"

#include <stdio.h>

Renderer* create_renderer(BaboonWindow* window)
{
    if (!window)
        return NULL;

    D3D12Renderer *renderer = malloc(sizeof(D3D12Renderer));
    if (!renderer)
        return NULL;

    HWND hwnd = get_win32_window(window);
    int width, height;
    get_window_size(window, &width, &height);

    // Create DXGI factory
    IDXGIFactory4 *factory = NULL;
    CreateDXGIFactory1(&IID_IDXGIFactory4, (void**)&factory);

    // Create device
    ID3D12Device *device = NULL;
    D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_12_1, &IID_ID3D12Device, (void**)&device);

    // Create command queue
    D3D12_COMMAND_QUEUE_DESC queue_desc = {
        .Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
    };
    ID3D12CommandQueue *command_queue = NULL;
    device->lpVtbl->CreateCommandQueue(device, &queue_desc, &IID_ID3D12CommandQueue, (void**)&command_queue);

    // Create swap chain
    DXGI_SWAP_CHAIN_DESC1 swap_chain_desc = 
    {
        .Width = width,
        .Height = height,
        .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .Stereo = FALSE,
        .SampleDesc = {.Count = 1, .Quality = 0},
        .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
        .BufferCount = 2,
        .Scaling = DXGI_SCALING_STRETCH,
        .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
        .AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
        .Flags = 0,
    };

    IDXGISwapChain1 *swap_chain1 = NULL;
    factory->lpVtbl->CreateSwapChainForHwnd(
        factory,
        (IUnknown*)command_queue,
        hwnd,
        &swap_chain_desc,
        NULL,
        NULL,
        &swap_chain1
    );

    IDXGISwapChain4 *swap_chain = NULL;
    swap_chain1->lpVtbl->QueryInterface(swap_chain1, &IID_IDXGISwapChain4, (void**)&swap_chain);
    swap_chain1->lpVtbl->Release(swap_chain1);

    renderer->device = device;
    renderer->swap_chain = swap_chain;
    renderer->command_queue = command_queue;

    factory->lpVtbl->Release(factory);

    return (Renderer*)renderer;
}

void renderer_clear(Renderer* renderer, float r, float g, float b, float a)
{
    (void)r;
    (void)g;
    (void)b;
    (void)a;
    D3D12Renderer* d3d12renderer = (D3D12Renderer*)renderer;
    d3d12renderer->swap_chain->lpVtbl->Present(d3d12renderer->swap_chain, 1, 0);
}

void renderer_present(Renderer* renderer, BaboonWindow* window)
{
    (void)window;

    if (!renderer)
        return;

    D3D12Renderer* d3d12renderer = (D3D12Renderer*)renderer;
    d3d12renderer->swap_chain->lpVtbl->Present(d3d12renderer->swap_chain, 1, 0);
}

void destroy_renderer(Renderer* renderer)
{
    if (!renderer)
        return;

    D3D12Renderer* d3d12renderer = (D3D12Renderer*)renderer;

    if (d3d12renderer->swap_chain)
        d3d12renderer->swap_chain->lpVtbl->Release(d3d12renderer->swap_chain);
    if (d3d12renderer->command_queue)
        d3d12renderer->command_queue->lpVtbl->Release(d3d12renderer->command_queue);
    if (d3d12renderer->device)
        d3d12renderer->device->lpVtbl->Release(d3d12renderer->device);

    free(d3d12renderer);
}
