/*
 * event_generator.cpp
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

#include "event_generator.hpp"
#include <chrono>
#include <random>
#include <iostream>

#define EPSILON 1E-2

template <typename value_t>
Event_Generator<value_t>::Event_Generator(value_t lambda, value_t trap_efficiency, long seed):
lambda(lambda),
trap_efficiency(trap_efficiency)
{
    unsigned seed_final;
    if(seed<0) {
        seed_final = std::chrono::system_clock::now().time_since_epoch().count();
    } else {
        seed_final = seed;
    }

    //std::cout << "seed: " << seed_final << std::endl;
    generator.seed(seed_final);
}

//TODO generate position
template <typename value_t>
Event<value_t> Event_Generator<value_t>::generate(value_t t_max, value_t w_max)
{
    std::vector<value_t> timestamps;
    std::vector<value_t> w_vals;

    timestamps.push_back(generate_t0(t_max));
    w_vals.push_back(value_t(0));
    w_vals.push_back(generate_w(value_t(0), w_max));

    do {
        int N=timestamps.size();
        value_t t_old = timestamps[N-1];
        value_t t = next_timestamp(t_old);
        timestamps.push_back(t);

        if(!has_left_trap())
            break;

        value_t w_old = w_vals[N];
        value_t w = new_frequency(t, t_old, w_old, w_max);

        if(w>w_max)
            break;

        w_vals.push_back(w);
    } while(true);

    //std::cout << "count: " << count << std::endl;
    w_vals.push_back(value_t(0));

    std::cout << "#scatter events: \n";

    for(int i=0; i<timestamps.size(); ++i)
        std::cout << "# t" << timestamps[i] << " f " << w_vals[i+1] << "\n";

    //return Event<value_t>(value_t(0), value_t(0), std::move(timestamps), std::move(w_vals));
    return {value_t(0), value_t(0), std::move(timestamps), std::move(w_vals)};
}

template <typename value_t>
value_t Event_Generator<value_t>::generate_t0(value_t t_max)
{
    std::uniform_real_distribution<value_t> dis(0.0, 1.0);
    value_t rand_val = dis(generator);

    return rand_val*t_max;
}

template <typename value_t>
value_t Event_Generator<value_t>::generate_w(value_t w_min, value_t w_max)
{
    std::uniform_real_distribution<value_t> dis(0.0, 1.0);
    value_t rand_val = dis(generator);

    return rand_val*(w_max-w_min)+w_min;
}

template <typename value_t>
value_t Event_Generator<value_t>::generate_E(value_t E_max)
{
    std::uniform_real_distribution<value_t> dis(0.0, 1.0);
    value_t rand_val = dis(generator);

    return rand_val*E_max;
}

template <typename value_t>
bool Event_Generator<value_t>::has_left_trap()
{
    std::uniform_real_distribution<value_t> dis(0.0, 1.0);
    value_t rand_val = dis(generator);

    //std::cout << "randval: " << rand_val << std::endl;

    //assuming isotropic scattering
    return rand_val<trap_efficiency;
}

template <typename value_t>
value_t Event_Generator<value_t>::next_timestamp(value_t t_old)
{
    std::exponential_distribution<value_t> dis(lambda);

    return t_old + dis(generator);
}

template <typename value_t>
value_t Event_Generator<value_t>::new_frequency(value_t t, value_t t_old,
                                                value_t w_old, value_t w_max)
{
    //std::exponential_distribution<value_t> dis(lambda);

    //value_t w = Event<float>::calc_w(t, t_old, w_old) + dis(generator);

    value_t w_now = Event<float>::calc_w(t, t_old, w_old);

    value_t E_now = 1/w_now;

    value_t E_new = generate_E(E_now);

    value_t w_new = 1/E_new;

    //~ value_t w {0};
    //~ value_t diff = w_max-w_now;

    //~ if(diff*diff>EPSILON)
        //~ w = generate_w(w_now, w_max);

    std::cout << "#" << w_now << " " << w_new << " " << w_max << "\n";
    return w_new;
}

template class Event_Generator<float>;
template class Event_Generator<double>;