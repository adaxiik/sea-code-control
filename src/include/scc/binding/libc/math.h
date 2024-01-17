#pragma once

namespace scc::binding::libc
{

constexpr auto math_h = R"(

float _scc_sinf(float x);
float _scc_cosf(float x);
double _scc_sin(double x);
double _scc_cos(double x);


float sinf(float x)
{
    return _scc_sinf(x);
}

float cosf(float x)
{
    return _scc_cosf(x);
}

double sin(double x)
{
    return _scc_sin(x);
}

double cos(double x)
{
    return _scc_cos(x);
}

)";

}