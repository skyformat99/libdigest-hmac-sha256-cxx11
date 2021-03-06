#include <cstdint>
#include <string>
#include <array>
#include <algorithm>
#include <utility>
#include <stdexcept>
#include "cipher-aes-gcm.hpp"
#include "cipher-aes.hpp"
#include "digest-ghash.hpp"

namespace cipher {

AES_GCM::AES_GCM (void) : ghash (), aes ()
{
    clear ();
}

AES_GCM&
AES_GCM::set_key128 (std::array<std::uint8_t,16> const& key128)
{
    aes.set_encrypt_key128 (key128);
    set_ghash_key ();
    return *this;
}

AES_GCM&
AES_GCM::set_key192 (std::array<std::uint8_t,24> const& key192)
{
    aes.set_encrypt_key192 (key192);
    set_ghash_key ();
    return *this;
}

AES_GCM&
AES_GCM::set_key256 (std::array<std::uint8_t,32> const& key256)
{
    aes.set_encrypt_key256 (key256);
    set_ghash_key ();
    return *this;
}

void
AES_GCM::set_ghash_key (void)
{
    AES::BLOCK zero {{0}};
    AES::BLOCK hash_key;
    aes.encrypt (zero, hash_key);
    ghash.set_key128 (hash_key);
}

AES_GCM&
AES_GCM::clear (void)
{
    authdata.clear ();
    nonce.clear ();
    expected_tag.clear ();
    state = INIT;
    pos = 0;
    return *this;
}

AES_GCM&
AES_GCM::add_authdata (std::string const& a)
{
    authdata = a;
    return *this;
}

AES_GCM&
AES_GCM::set_nonce (std::string const& a)
{
    nonce = a;
    return *this;
}

AES_GCM&
AES_GCM::set_authtag (std::string const& a)
{
    expected_tag = a;
    return *this;
}

AES_GCM&
AES_GCM::encrypt (void)
{
    reset_counter ();
    tag.clear ();
    ghash.set_authdata (authdata);
    state = ENCRYPT;
    return *this;
}

std::string
AES_GCM::authtag (void)
{
    if (ENCRYPT == state || DECRYPT == state) {
        tag = ghash.digest ();
        for (int i = 0; i < tag.size (); ++i)
            tag[i] = static_cast<std::uint8_t> (tag[i]) ^ key_stream0[i];
        state = FINAL;
    }
    return tag;
}

AES_GCM&
AES_GCM::decrypt (void)
{
    encrypt ();
    state = DECRYPT;
    return *this;
}

bool
AES_GCM::good (void)
{
    // constant-time comparison while tag == expected_tag
    authtag ();
    bool ok = true;
    for (int i = 0; i < tag.size (); ++i) {
        volatile bool const prev_ok = ok;
        volatile bool const not_ok = false;
        int const expected_c = i >= expected_tag.size () ? 0 : expected_tag[i];
        ok = tag[i] == expected_c ? prev_ok : not_ok;
    }
    return ok;
}

std::string
AES_GCM::update (std::string::const_iterator s, std::string::const_iterator e)
{
    if (ENCRYPT != state && DECRYPT != state)
        throw std::runtime_error ("update() decends encrypt() or decrypt().");
    if (s >= e)
        return "";
    if (DECRYPT == state)
        ghash.add (s, e);
    std::string dst;
    while (s < e) {
        dst.push_back (static_cast<std::uint8_t> (*s++) ^ key_stream[pos]);
        if (++pos >= key_stream.size ()) {
            increment_counter ();
            pos = 0;
        }
    }
    if (ENCRYPT == state)
        ghash.add (dst.cbegin (), dst.cend ());
    return std::move (dst);
}

std::string
AES_GCM::update (std::string const& src)
{
    return update (src.cbegin (), src.cend ());
}

void
AES_GCM::reset_counter (void)
{
    if (nonce.size () == 12) {
        std::copy (nonce.cbegin (), nonce.cend (), counter.begin ());
        counter[12] = 0;
        counter[13] = 0;
        counter[14] = 0;
        counter[15] = 1;
    }
    else {
        ghash.set_authdata ("");
        std::string h = ghash.add (nonce).digest ();
        std::copy (h.cbegin (), h.cend (), counter.begin ());
    }
    aes.encrypt (counter, key_stream0);
    increment_counter ();
}

void
AES_GCM::increment_counter (void)
{
    // constant-time increment
    AES::BLOCK::value_type carry = 1U;
    for (int i = counter.size () - 1; i >= 0; --i) {
        counter[i] += carry;
        carry = counter[i] < carry ? 1U : 0;
    }
    aes.encrypt (counter, key_stream);
}

}//namespace cipher

/* Copyright (c) 2016, MIZUTANI Tociyuki
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
