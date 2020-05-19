#include <ArduinoOSC.h>

void hexdump(const uint8_t* s_void, size_t sz, size_t offset)
{
    unsigned char *s = (unsigned char*)s_void;
    size_t nb = 16;
    String info;
    for (size_t l=0; l < sz; l += nb)
    {
        char tmp[12];
        sprintf(tmp, "%08x ", (uint32_t)(l+offset));
        info += String(tmp);
        for (size_t c = 0; c < nb; ++c)
        {
            if (l+c < sz)
            {
                sprintf(tmp, "%02x ", s[l+c]);
                info += String(tmp);
            }
            else
                info += String("   ");
        }
        info += String("| ");
        for (size_t c = 0; c < nb; ++c)
        {
            if (c+l < sz)
            {
                if (s[l+c] >= ' ' && s[l+c] < 127)
                    info += String(s[l+c]);
                else
                    info += String(".");
            } else
                info += String(" ");
        }
        Serial.println();
    }
    Serial.println(info);
}

void basicTests()
{
    OscMessage msg;

    OscEncoder wr;
    msg.init("/foot");
    wr.init().encode(msg);

    Serial.println("simplest message encode :");
    hexdump(wr.data(), wr.size(), 0);
    Serial.println();

    wr.init().begin_bundle();
    wr.encode(msg.init("/foo").push(1000).push(-1).push("hello").push(1.234f).push(5.678f));
    wr.end_bundle();

    Serial.println("larger message encode :");
    hexdump(wr.data(), wr.size(), 0);
    Serial.println((wr.size() == 0x3c &&
            memcmp(wr.data(),
                    "\x23\x62\x75\x6e\x64\x6c\x65\x00\x00\x00\x00\x00\x00\x00\x00\x01"
                    "\x00\x00\x00\x28\x2f\x66\x6f\x6f\x00\x00\x00\x00\x2c\x69\x69\x73"
                    "\x66\x66\x00\x00\x00\x00\x03\xe8\xff\xff\xff\xff\x68\x65\x6c\x6c"
                    "\x6f\x00\x00\x00\x3f\x9d\xf3\xb6\x40\xb5\xb2\x2d", wr.size()) == 0) ? "Success" : "Failed");

    OscDecoder pr(wr.data(), wr.size());

    Serial.println("argument decode test: ");
    OscMessage *mr = pr.decode();
    {
        int i1 = mr->arg<int32_t>(0);
        int i2 = mr->arg<int32_t>(1);
        String s = mr->arg<String>(2);
        float f1 = mr->arg<float>(3);
        float f2 = mr->arg<float>(4);

        Serial.print("int1   : "); Serial.println((i1 == 1000) ? "Success" : "Failed");
        Serial.print("int2   : "); Serial.println((i2 == -1) ? "Success" : "Failed");
        Serial.print("String : "); Serial.println((s == "hello") ? "Success" : "Failed");
        Serial.print("float1 : "); Serial.println((f1 == 1.234f) ? "Success" : "Failed");
        Serial.print("float2 : "); Serial.println((f2 == 5.678f) ? "Success" : "Failed");
    }

    wr.init().begin_bundle().begin_bundle().end_bundle().end_bundle();
    hexdump(wr.data(), wr.size(), 0);
    Serial.print("Bundle encode : ");
    Serial.println((wr.size() == 40) ? "Success" : "Failed");

    pr.init(wr.data(), wr.size());
    Serial.print("Bundle decode : ");
    Serial.println((pr.decode() == 0) ? "Success" : "Failed");
}

void checkMatch(const char *pattern, const char *test, bool expected_match = true)
{
    Serial.print("doing fullPatternMatch('");
    Serial.print(pattern);
    Serial.print("', '");
    Serial.print(test);
    Serial.print("'), expected result is : ");
    Serial.print(expected_match ? "MATCH" : "MISMATCH");
    Serial.println();

    bool m = ArduinoOSC::match(pattern, test);
    if (!expected_match)
    {
        if (m) { Serial.print("unexpected match... "); Serial.print(pattern); Serial.print(" with "); Serial.println(test); }
    }
     else
    {
        if (!m) { Serial.print("unexpected mismatch... "); Serial.print(pattern); Serial.print(" with "); Serial.println(test); }
        String tmp(test);
        while (tmp.length() && tmp[tmp.length()-1] != '/') tmp.remove(tmp.length()-1);
        assert(ArduinoOSC::match(pattern, tmp, false));
    }
}

void patternTests()
{
    Serial.println("Doing pattern tests");
    checkMatch("//bar", "bar", false);
    checkMatch("//bar", "/bar");
    checkMatch("//bar", "/foo/plop/bar");
    checkMatch("/foo//", "/foo/plop/df/");
    checkMatch("/foo///////bar", "/foo/plop/baz/bar");
    checkMatch("*", "bar");
    checkMatch("/foo/*", "/foo/bar");
    checkMatch("/{bar,fo}/b[aA]r", "/fo/bar");
    checkMatch("/{bar,fo}/b[aA]r", "/foo/bar", false);
    checkMatch("/fo{bar,}/ba[e-t]", "/fo/bar");
    checkMatch("/fo{bar,}/ba[t-z]", "/fo/bar", false);
    checkMatch("/f{,ioio,bar}o/?a[!a]", "/fo/bar");
    checkMatch("/foo/bar", "/foo/bar");
    checkMatch("/f*o/bar", "/foo/bar");
    checkMatch("/fo*o/bar", "/foo/bar");
    checkMatch("/*//bar", "/foo/bar");
    checkMatch("/*/bar", "/foo/bar");
    checkMatch("/*o/bar", "/foo/bar");
    checkMatch("/*/*/*/*a***/*/*/*/*/", "/foo/bar/foo/barrrr/foo/bar/foo/barrrr/");
    checkMatch("/*/*/*/**/*/*/*/*/q", "/foo/bar/foo/barrrr/foo/bar/foo/barrrr/p", false);
}

void setup()
{
    delay(2000);

    Serial.begin(115200);
    Serial.println("start");

    OscDecoder pr;
    OscEncoder pw;

    // osc message
    Serial.println("test message");
    {
        OscMessage message;
        // message.init("/ping").pushInt32(10).pushString("test").pushFloat(11.123);
        message.init("/ping").push(10).push("test").push(4.321F).push((double)11.1234);
        pw.init().encode(message);

        pr.init(pw.data(), pw.size());
        while (OscMessage *msg = pr.decode())
        {
            if (msg->match("/ping"))
            {
                Serial.print("unpacked ");
                Serial.print(msg->getArgAsInt32(0)); Serial.print(", ");
                Serial.print(msg->getArgAsString(1)); Serial.print(", ");
                Serial.print(msg->getArgAsFloat(2)); Serial.print(", ");
                Serial.print(msg->getArgAsDouble(3)); Serial.println();

// #ifndef __AVR__
                Serial.print("unpacked ");
                Serial.print(msg->arg<int32_t>(0)); Serial.print(", ");
                Serial.print(msg->arg<String>(1)); Serial.print(", ");
                Serial.print(msg->arg<float>(2)); Serial.print(", ");
                Serial.print(msg->arg<double>(3)); Serial.println();
// #endif

                // int i = msg->arg(0);
                // String s = msg->arg(1);
                // float f = msg->arg(2);
                // Serial.print("unpacked ");
                // Serial.print(i); Serial.print(", ");
                // Serial.print(s); Serial.print(", ");
                // Serial.print(f); Serial.println();
            }
            else
                Serial.println("Server: unhandled message: ");
        }
    }

    // osc blob
    Serial.println("test blob");
    {
        uint8_t blob[3] = {1, 2, 3};
        OscMessage message;
        message.init("/blob").pushBlob(blob, sizeof(blob));
        pw.init().encode(message);

        pr.init(pw.data(), pw.size());
        while (OscMessage* msg = pr.decode())
        {
            OscBlob rep;
            if (msg->match("/blob")) {
                Serial.print("unpacked ");
                rep = msg->getArgAsBlob(0);
                for (uint8_t i = 0; i < rep.size(); ++i)
                {
                    Serial.print((int)rep[i]); Serial.print(", ");
                }
                Serial.println();
            } else {
                Serial.println("Server: unhandled message: ");
            }
        }
    }

    Serial.println("finished");

    basicTests();
    patternTests();
}

void loop()
{

}
