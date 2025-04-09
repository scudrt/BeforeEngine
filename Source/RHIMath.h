#pragma once

#ifdef RHI_DX12

#include <DirectXMath.h>
/*
* Basic vector type definitions
*/
using vec2i = DirectX::XMINT2;
using vec3i = DirectX::XMINT3;
using vec4i = DirectX::XMINT4;
using vec2 = DirectX::XMFLOAT2;
using vec3 = DirectX::XMFLOAT3;
using vec4 = DirectX::XMFLOAT4;

#endif