#include <windows.h>
#include <d3d11_1.h>

int globalRunning = 1;

LRESULT CALLBACK 
WindowProc(HWND window, 
           UINT message, 
           WPARAM wParam, 
           LPARAM lParam)
{
    LRESULT result;
    switch(message)
    {
        case WM_CLOSE:
        {
            globalRunning = 0;
        } break;
        
        default:
        {
            result = DefWindowProc(window, message, wParam, lParam);
        } break;
    }
    
    return result;
}

int WINAPI 
wWinMain(HINSTANCE instance, 
         HINSTANCE prevInstance, 
         PWSTR cmdLine, 
         int cmdShow)
{
    WNDCLASS window_class = {0};
    
    const wchar_t CLASS_NAME[] = L"GameWindowClass";
    
    window_class.lpfnWndProc = WindowProc;
    window_class.hInstance = instance;
    window_class.lpszClassName = CLASS_NAME;
    
    RegisterClass(&window_class);
    
    HWND window = CreateWindowEx(0,
                                 CLASS_NAME,
                                 L"Game",
                                 WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                                 CW_USEDEFAULT,
                                 CW_USEDEFAULT,
                                 CW_USEDEFAULT,
                                 CW_USEDEFAULT,
                                 0,
                                 0,
                                 instance,
                                 0);
    
    
    
    ID3D11Device *base_device;
    ID3D11DeviceContext *base_context;
    
    D3D_FEATURE_LEVEL feature_levels[] = {
        D3D_FEATURE_LEVEL_11_0
    };
    
    D3D11CreateDevice(0,
                      D3D_DRIVER_TYPE_HARDWARE,
                      0,
                      0,
                      feature_levels,
                      1,
                      D3D11_SDK_VERSION,
                      &base_device,
                      0,
                      &base_context
                      );
    
    
    ID3D11Device1 *device;
    ID3D11DeviceContext1 *context;
    
    base_device->QueryInterface(__uuidof(ID3D11Device1), (void **)&device);
    base_context->QueryInterface(__uuidof(ID3D11DeviceContext1), (void **)&context);
    
    
    // swap chain
    
    DXGI_SWAP_CHAIN_DESC1 swap_chain_desc = {};
    
    swap_chain_desc.Width = 0;
    swap_chain_desc.Height = 0;
    swap_chain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chain_desc.SampleDesc.Count = 1;
    swap_chain_desc.SampleDesc.Quality = 0;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.BufferCount = 1;
    swap_chain_desc.Scaling = DXGI_SCALING_STRETCH;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swap_chain_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    swap_chain_desc.Flags = 0;
    
    IDXGISwapChain1 *swap_chain;
    
    IDXGIDevice2 * dxgi_device;
    device->QueryInterface(__uuidof(IDXGIDevice2), (void **)&dxgi_device);
    
    IDXGIAdapter * dxgi_adapter;
    dxgi_device->GetParent(__uuidof(IDXGIAdapter), (void **)&dxgi_adapter);
    
    IDXGIFactory2 * dxgi_factory;
    dxgi_adapter->GetParent(__uuidof(IDXGIFactory2), (void **)&dxgi_factory);
    
    dxgi_factory->CreateSwapChainForHwnd(device,
                                         window,
                                         &swap_chain_desc,
                                         0,
                                         0,
                                         &swap_chain
                                         );
    
    ID3D11Texture2D *back_buffer;
    swap_chain->GetBuffer(0,
                          __uuidof(ID3D11Texture2D),
                          (void **)&back_buffer
                          );
    
    ID3D11RenderTargetView *render_target_view;
    
    device->CreateRenderTargetView(back_buffer,
                                   0,
                                   &render_target_view
                                   );
    
    while(globalRunning)
    {
        MSG message;
        while(PeekMessage(&message, window, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
        
        float color[4] = {1.0f, 0.0f, 0.0f, 1.0};
        
        context->ClearRenderTargetView(render_target_view, color);
        
        swap_chain->Present(0, 0);
    }
    
    return 0;
}
