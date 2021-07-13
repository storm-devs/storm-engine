#pragma once

#include "Cvector.h"
#include "entity.h"

#define MAX_FLOAT_PARAMS 128
#define MAX_LONG_PARAMS 64
#define MAX_COLOR_PARAMS 32
#define MAX_VECTORS_PARAMS 32
#define MAX_STRINGS_PARAMS 32

// define float parameters

#define whf_fog_start 0
#define whf_fog_density 1

#define whf_bump_tile 5
#define whf_bump_ang 6
#define whf_bump_speed 7
#define whf_bump_anim_speed 8

#define whf_water_transparency 14
#define whf_frenel_coefficient 15
#define whf_water_reflection 16
#define whf_water_attenuation 17

#define whf_sun_azimuth_angle 22
#define whf_sun_height_angle 23
#define whf_sun_road_multiply 24
#define whf_sun_shadow 25

#define whf_wind_angle 30
#define whf_wind_speed 31

#define whf_rain_drop_length 36
#define whf_rain_height 37
#define whf_rain_radius 38
#define whf_rain_speed 39
#define whf_rain_jitter 40
#define whf_rain_wind_speed_jitter 41

#define whf_sun_glow_size 42
#define whf_sky_speed 43
#define whf_sun_road_substract 44

#define whf_time_counter 100
#define whf_time_speed 101

// define long parameters

#define whi_weather_update 0 // if weather.ini update this flag one cycle is 1
#define whi_fog_enable 1
#define whi_harmonics_num 2
#define whi_rain_drops_num 3
#define whi_rain_time_blend 4
#define whi_rain_max_blend 5
#define whi_sky_number 6
#define whi_sun_glow_enable 7
#define whi_lightning_enable 8
#define whi_lightning_freq 9
#define whi_rainbow_enable 10

// define colors parameters

#define whc_sky_color 0
#define whc_water_color 1
#define whc_sun_color 2
#define whc_fog_color 3
#define whc_rain_color 4
#define whc_pena_color 5
#define whc_sun_glow_color 6
#define whc_sun_ambient 7

// define vector parameters

#define whv_sun_pos 0

// define harmonics data

// define strings parameters
#define whs_sunglow_texture 0
#define whs_rainbow_texture 1

class WEATHER_BASE : public Entity
{
  public:
    WEATHER_BASE(){};

    virtual ~WEATHER_BASE(){};

    virtual long GetLong(uint32_t dwCode) = 0;
    virtual uint32_t GetColor(uint32_t, CVECTOR *) = 0;
    virtual uint32_t GetColor(uint32_t) = 0;
    virtual float GetFloat(uint32_t) = 0;
    virtual void GetVector(uint32_t, CVECTOR *) = 0;
};
