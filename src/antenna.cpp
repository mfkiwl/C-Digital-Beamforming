/*
 * antenna.cpp
 *
 * Copyright 2020 Florian Thomas <>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */


#include "antenna.hpp"

template <typename value_t>
Antenna<value_t>::Antenna(  value_t snr,
                            value_t sample_rate,
                            value_t w_mix,
                            value_t x,
                            value_t y):
	snr(snr),
	sample_rate(sample_rate),
	w_mix(w_mix),
    x(x),
    y(y)
{}

template <typename value_t>
arma::Col<value_t> Antenna<value_t>::sample_noise(int n_samples)
{
    //std::cout << "sample noise" << std::endl;

    arma::Col<value_t> noise = arma::randn<arma::Col<value_t>>(n_samples);

    return noise;
}

template <typename value_t>
value_t Antenna<value_t>::distance_to_electron(const Electron<value_t>& e)
{
    //std::cout << "dist to electron" << std::endl;

    value_t dx = x-e.x;
    value_t dy = y-e.y;
    return sqrt(dx*dx + dy*dy);
}

template <typename value_t>
value_t Antenna<value_t>::time_delay(const Electron<value_t>& e)
{
    //std::cout << "time delay" << std::endl;

    value_t dist = this->distance_to_electron(e);
    value_t c = arma::datum::c_0*100.0f;
    //std::cout << c << std::endl;
    return dist/c;
}

template <typename value_t>
value_t Antenna<value_t>::spiral_phase(const Electron<value_t>& e)
{
    //std::cout << "spiral phase" << std::endl;

    value_t dx = x - e.x;
    value_t dy = y - e.y;
    value_t phi = atan2(-dy,-dx)+M_PI;

    //std::cout << "phi: " << phi << std::endl;
    return phi;
}

template <typename value_t>
arma::Col<value_t> Antenna<value_t>::get_mixed_sample(const arma::Col<value_t>& t,
                                                        const Electron<value_t>& e)
{
    //std::cout << "sample mixed" << std::endl;

    value_t delta_t = this->time_delay(e);
    value_t phi = this->spiral_phase(e);
    value_t dist = this->distance_to_electron(e);

    arma::Col<value_t> samples(t.n_elem);

    for (unsigned int i=0; i<t.n_elem; i++) {
        samples[i] = 1/dist*e.emission(t[i], e.w-w_mix, -delta_t*e.w-phi);
    }
    return samples;
}

template <typename value_t>
Sample<value_t> Antenna<value_t>::sample_data(int n_samples, value_t t0, const Electron<value_t>& e)

{
    //std::cout << "sample data" << std::endl;

    arma::Col<value_t> sample_times(n_samples);

    for(unsigned int i=0; i<n_samples; ++i) {
        sample_times[i]= i/sample_rate + t0;
    }

    arma::Col<value_t> signal = this->get_mixed_sample(sample_times, e);
    arma::Col<value_t> noise = this->sample_noise(n_samples);

    arma::Col<value_t> data = sqrt(snr)*signal+noise;

    return Sample<value_t>(sample_times, data);
}

template class Antenna<float>;
template class Antenna<double>;
