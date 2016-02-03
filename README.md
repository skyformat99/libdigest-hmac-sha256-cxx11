digest::SHA256, digest::HMAC, pbkdf2-sha256
======================================

SHA-256 and MD5 classes, and HMAC class template,
encode\_base64 function, decode\_base64 function,
pbkdf2\_sha256::encrypt function, and pbkdf2\_sha256::verify function
for C++11.

SYNOPSIS
--------

    #include "digest.hpp"
    digest::SHA256 digest_object;
    digest::MD5    digest_object;
    digest::HMAC<digest::SHA256> digest_object (std::string const& key);
    digest::base& digest_object.add (std::string const& data);
    std::string octets = digest_object.digest ();
    std::string hexlower = digest_object.hexdigest ();
    digest::base& digest_object.reset ();
    digest::base& digest_object.finish ();

    #include "mime-base64.hpp"
    // line wrap 76 columns with '\n'
    std::string base64 = mime::encode_base64 (std::string const& octets);
    // line wrap 76 columns with endline
    std::string base64 = mime::encode_base64 (std::string const& octets,
        std::string const& endline);
    std::string b64url = mime::encode_base64url (std::string const& octets);
    std::string b64crypt = mime::encode_base64crypt (std::string const& octets);
    bool mime::decode_base64 (std::string const& base64, std::string& octets);
    bool mime::decode_base64url (std::string const& b64url, std::string& octets);
    bool mime::decode_base64crypt (std::string const& b64crype, std::string& octets);

    #include "pbkdf2-sha256.hpp"
    // generate crypt hash with options rounds=6400U, salt_size=16U
    // the hash format is Python's passlib.hash.pbkdf2.pbkdf2_sha256
    std::string hash = pbkdf2_sha256::encrypt (std::string const& password);
    std::string hash = pbkdf2_sha256::encrypt (
        std::string const& password, std::size_t const rounds);
    std::string hash = pbkdf2_sha256::encrypt (
        std::string const& password, std::size_t const rounds,
        std::size_t const salt_size);
    std::string hash = pbkdf2_sha256::encrypt (
        std::string const& password, std::string const& salt);
    std::string hash = pbkdf2_sha256::encrypt (
        std::string const& password, std::size_t const rounds,
        std::string const& salt);
    bool good = pbkdf2_sha256::verify (
        std::string const& password, std::string const& hash);

DESCRIPTION
-----------

To calculate a SHA-256 message digest, use SHA256 class.

To calculate a MD5 message digest, use MD5 class.

To calculate a HMAC message authentication code, use HMAC class
template. Its constructor creates the digest object with
a key argument as a std::string. It uses the key over the
object life time. Currently, there is no method changing key
settings.

They calculate from the sequences of byte-oriented input data
as a std::string to call add member function repeatedly.
After the sequences, call hexdigest or digest member function
to get a message digest or a message authuncitation code
as a std::string. Once after calling digest or hexdigest member
function, the digest object teminates the previous input data
sequences. So that calling add member function on termination
starts new sequences of input data to calculate another code.
To terminate input sequences explicitly, call finish member
function. It calls automatically before digest or hexdigest
member function. To discard data input sequences, call reset
member function. It initialises the digest object as same as
the situation just creating it.

Current version accepts only byte-oriented input data.
Bit-oriented data are not available.

The representation of the output vector is octets by digest
member function. One is lowercase hexdecimals by hexdigest
member function.

EXAMPLES
--------

    #include "digest.hpp"
    #include <string>
    #include <iostream>
    
    void
    example_sha256 ()
    {
        // Authorization: Digest algorithm=SHA-256,qop=auth,...
        digest::SHA256 h1;
        digest::SHA256 h2;
        digest::SHA256 hr;
        std::string a1     = "Mufasa:http-auth@example.org:Circle of Life";
        std::string a2     = "GET:/dir/index.html";
        std::string nonce  = "7ypf/xlj9XXwfDPEoM4URrv/xwf94BcCAzFZH4GiTo0v";
        std::string nc     = "00000001";
        std::string cnonce = "f2/wE4q74E6zIJEtWaHKaf5wv/H5QzzpXusqGemxURZJ";
        std::string got = hr.add (h1.add (a1).hexdigest ())
                            .add (":").add (nonce)
                            .add (":").add (nc)
                            .add (":").add (cnonce)
                            .add (":auth:").add (h2.add (a2).hexdigest ())
                            .hexdigest ();
        //=> 753927fa0e85d155564e2e272a28d1802ca10daf4496794697cf8db5856cb6c1
    }

    void
    example_hmac_sha256 ()
    {
        // RFC 4231 HMAC-SHA256 test case 2
        std::string key = "Jefe";
        std::string data = "what do ya want for nothing?";
        digest::HMAC<digest::SHA256> hmac (key);
        std::string got = hmac.add (data).hexdigest ();
        //=> 5bdcc146bf60754e6a042426089575c75a003f089d2739839dec58b964ec3843
    }

COPYRIGHT AND LICENSE
---------------------

Copyright (c) 2015, MIZUTANI Tociyuki  
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
 3. Neither the name of the copyright holder nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.