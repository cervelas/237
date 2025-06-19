#ifndef __UI_H
#define __UI_H

#include <ArduinoHttpServer.h>

#include "WiFiS3.h"

void render_midi_ctrl(WiFiClient client) {
    String tpl = F(                                                         //
        "<div>DEBUG "                                                       //
        "<a href='/debug_cc'>CC</a>&nbsp;"                                  //
        "<a href='/debug_sensor'>sensor</a>&nbsp;"                          //
        "<a href='/debug_raw'>raw</a>&nbsp;"                                //
        "<a href='/debug_midi'>midi</a>&nbsp;"                              //
        "</div>"                                                            //
        "<form method='post' action='/midi'><fieldset>"                     //
        "<div class='float-right'>"                                         //
        "<input type='submit' value='Update'>"                              //
        "</div>"                                                            //
        "<h3>MIDI Control</h3>"                                             //
        "<div>"                                                             //
        "MIDI <b>{{connect_status}}</b>&nbsp;"                              //
        "<a href='/connect'>connect</a>&nbsp;"                              //
        "<a href='/end'>end</a>"                                            //
        "</div>"                                                            //
        "<div>"                                                             //
        "SEND <b>{{midi_status}}</b>&nbsp;"                                 //
        "<a href='/enable'>Enable</a>&nbsp;"                                //
        "<a href='/disable'>Disable</a>"                                    //
        "</div>"                                                            //
        "<label for='cc_smin'>Remote IP Address {{remote_prefix}}</label>"  //
        "<input type='number' id='remote_id' name='remote_id' "             //
        "value='{{remote_id}}' min=0 max=255>"                              //
        "</fieldset></form>"                                                //
        "<hr>"                                                              //
    );                                                                      //

    tpl.replace("{{localip}}", WiFi.localIP().toString());
    tpl.replace("{{remote_prefix}}", String(String(WiFi.localIP()[0]) + "." +
                                            String(WiFi.localIP()[1]) + "." +
                                            String(WiFi.localIP()[2]) + "."));
    tpl.replace("{{remote_id}}", String(midi_remote_id.get()));
    if (udp_connected)
        tpl.replace("{{connect_status}}", "CONNECTED");
    else
        tpl.replace("{{connect_status}}", "DISCONNECTED");

    if (midi_enable)
        tpl.replace("{{midi_status}}", "ENABLE");
    else
        tpl.replace("{{midi_status}}", "DISABLED");

    client.println(tpl);
}

void midi_get_handler(String url) {
    if (url.endsWith("enable"))
        midi_enable = true;
    else if (url.endsWith("disable"))
        midi_enable = false;
}

void debug_get_handler(String url) {
    if (url.endsWith("debug_cc"))
        debug_cc = true;
    else if (url.endsWith("debug_sensor"))
        debug_sensor = true;
    else if (url.endsWith("debug_raw"))
        debug_raw = true;
    else if (url.endsWith("debug_midi"))
        debug_midi = true;
}

void midi_post_handler(String url, String name, String value) {
    if (url.endsWith("midi")) {
        if (name.compareTo("remote_id") == 0) {
            Serial.println("update remote id");
            midi_remote_id.set(atoi(value.c_str()));
        }
    }
}

void render_cc_ctrl(EEPROMStorage<uint16_t> smin, EEPROMStorage<uint16_t> smax,
                    EEPROMStorage<uint16_t> tmin, EEPROMStorage<uint16_t> tmax,
                    WiFiClient client) {
    String tpl = F(                                    //
        "<form method='post' action='/cc'><fieldset>"  //
        "<div class='float-right'><input type='submit' "
        "value='Update'></div>"  //
        "<h3>MIDI CC</h3>"       //

        "<label for='cc_smin'>Source MIN CC</label>"         //
        "<input type='number' id='cc_smin' name='cc_smin' "  //
        "value='{{cc_smin}}' min=0 max=1000>"                //

        "<label for='cc_smax'>Source MAX CC</label>"         //
        "<input type='number' id='cc_smax' name='cc_smax' "  //
        "value='{{cc_smax}}' min=0 max=1000>"                //

        "<label for='cc_tmin'>Target MIN CC</label>"         //
        "<input type='number' id='cc_tmin' name='cc_tmin' "  //
        "value='{{cc_tmin}}' min=0 max=127>"                 //

        "<label for='cc_tmax'>Target MAX CC</label>"         //
        "<input type='number' id='cc_tmax' name='cc_tmax' "  //
        "value='{{cc_tmax}}' min=0 max=127>"                 //

        "</fieldset></form>"  //
        "<hr>"                //
    );                        //

    tpl.replace("{{cc_smin}}", String(smin.get()));
    tpl.replace("{{cc_smax}}", String(smax.get()));
    tpl.replace("{{cc_tmin}}", String(tmin.get()));
    tpl.replace("{{cc_tmax}}", String(tmax.get()));

    client.println(tpl);
}

void cc_post_handler(String url, String name, String value) {
    if (url.endsWith("cc")) {
        if (name.compareTo("cc_smin") == 0)
            cc_smin.set(atoi(value.c_str()));

        if (name.compareTo("cc_smax") == 0)
            cc_smax.set(atoi(value.c_str()));

        if (name.compareTo("cc_tmin") == 0)
            cc_tmin.set(atoi(value.c_str()));

        if (name.compareTo("cc_tmax") == 0)
            cc_tmax.set(atoi(value.c_str()));
    }
}

void render_note_ctrl(int id, uint16_t trig_min, uint16_t trig_max,
                      uint16_t note, WiFiClient client) {
    String tpl = F(                                             //
        "<form method='post' action='/note{{uid}}'><fieldset>"  //
        "<div class='float-right'><input type='submit' "
        "value='update'></div>"   //
        "<h3>MIDI {{name}}</h3>"  //

        "<input type='hidden' id='uid' name='uid' value='{{uid}}'>"  //

        "<label for='note'>Note</label>"  //
        "<input type='number' id='note' name='note' value='{{note}}' min=0 "
        "max=127>"  //

        "<label for='trig_min'>MIN DIST</label>"  //
        "<input type='number' id='trig_min' name='trig_min' "
        "value='{{trig_min}}' min=0 max=500>"  //

        "<label for='trig_max'>MAX DIST</label>"  //
        "<input type='number' id='trig_max' name='trig_max' "
        "value='{{trig_max}}' min=0 max=500>"  //

        "</fieldset></form>"  //
        "<hr>"                //
    );                        //

    tpl.replace("{{uid}}", String(id));
    tpl.replace("{{name}}", String(id));
    tpl.replace("{{trig_min}}", String(trig_min));
    tpl.replace("{{trig_max}}", String(trig_max));
    tpl.replace("{{note}}", String(note));

    client.println(tpl);
}

void render_near_note_ctrl(uint16_t trig, uint16_t note, WiFiClient client) {
    String tpl = F(                                           //
        "<form method='post' action='/near_note'><fieldset>"  //
        "<div class='float-right'><input type='submit' "
        "value='update'></div>"    //
        "<h3>MIDI NEAR NOTE</h3>"  //

        "<label for='note'>Note</label>"  //
        "<input type='number' id='note' name='note' value='{{note}}' min=0 "
        "max=127>"  //

        "<label for='trig'>MAX DIST</label>"                            //
        "<input type='number' id='trig' name='trig' value='{{trig}}'>"  //

        "</fieldset></form>"  //
        "<hr>"                //
    );                        //

    tpl.replace("{{trig}}", String(trig));
    tpl.replace("{{note}}", String(note));

    client.println(tpl);
}

void render_far_note_ctrl(uint16_t trig, uint16_t note, WiFiClient client) {
    String tpl = F(                                          //
        "<form method='post' action='/far_note'><fieldset>"  //
        "<div class='float-right'><input type='submit' "
        "value='update'></div>"   //
        "<h3>MIDI FAR NOTE</h3>"  //

        "<label for='note'>Note</label>"  //
        "<input type='number' id='note' name='note' value='{{note}}' min=0 "
        "max=127>"  //

        "<label for='trig'>MIN DIST</label>"                            //
        "<input type='number' id='trig' name='trig' value='{{trig}}'>"  //

        "</fieldset></form>"  //
        "<hr>"                //
    );                        //

    tpl.replace("{{trig}}", String(trig));
    tpl.replace("{{note}}", String(note));

    client.println(tpl);
}

void cc_note_handler(String url, String name, String value) {
    if (url.startsWith("/note")) {
        String suid = String(url[5]);
        int id = atoi(suid.c_str());

        Note note = notes[id].get();

        logln(suid);
        logln(id);
        logln(url);
        logln(name);
        logln(value);

        if (name.compareTo("trig_min") == 0)
            note.min = atoi(value.c_str());
        if (name.compareTo("trig_max") == 0)
            note.max = atoi(value.c_str());
        if (name.compareTo("note") == 0)
            note.note = atoi(value.c_str());

        notes[id] = note;
    } else if (url.startsWith("/near_note")) {
        Note note = note_near.get();

        if (name.compareTo("trig") == 0)
            note.max = atoi(value.c_str());
        if (name.compareTo("note") == 0)
            note.note = atoi(value.c_str());

        note_near = note;

    } else if (url.startsWith("/far_note")) {
        Note note = note_far.get();

        if (name.compareTo("trig") == 0)
            note.min = atoi(value.c_str());
        if (name.compareTo("note") == 0)
            note.note = atoi(value.c_str());

        note_far = note;
    }
}

void render_ui_body(WiFiClient client) {
    render_midi_ctrl(client);

    render_cc_ctrl(cc_smin, cc_smax, cc_tmin, cc_tmax, client);

    render_near_note_ctrl(note_near.get().max, note_near.get().note, client);
    render_far_note_ctrl(note_far.get().min, note_far.get().note, client);

    for (int i = 0; i < NB_NOTES; i++) {
        Note note = notes[i].get();
        render_note_ctrl(i, note.min, note.max, note.note, client);
    }
}

void _debug_post(String url, String name, String value) {
    Serial.print("POST ");

    Serial.print(url);

    Serial.print(" ");

    Serial.print(name);
    Serial.print("=");
    Serial.println(value);
}

// Define number of post data callbacks, do not forget to sync with array below
// !
#define num_post_handlers 4

typedef void (*post_form_handler)(String, String, String);

// Define post data callbacks, do not forget to increment the num_handlers above
post_form_handler post_handlers[num_post_handlers] = {
    &_debug_post,
    &midi_post_handler,
    &cc_post_handler,
    &cc_note_handler,
};

void post_req_cb(ArduinoHttpServer::HttpResource resource, String body) {
    Serial.println(body);

    const char* arg_del = "&";
    const char* kv_del = "=";

    char arg_buff[200];
    body.toCharArray(arg_buff, 200);
    char kv_buff[40];

    char *tok, *saved;
    // process raw post form data as defined in RFC
    for (tok = strtok_r(arg_buff, arg_del, &saved); tok;
         tok = strtok_r(NULL, arg_del, &saved)) {
        String(tok).toCharArray(kv_buff, 40);
        String name = String(strtok(kv_buff, kv_del));
        String value = String(strtok(NULL, kv_del));
        for (int i = 0; i < num_post_handlers; ++i)
            post_handlers[i](resource.toString(), String(name), String(value));
    }
}

// Define number of get data callbacks, do not forget to sync with array below !
#define num_get_handlers 2

typedef void (*post_get_handler)(String);

post_get_handler get_handlers[num_get_handlers] = {&midi_get_handler,
                                                   &debug_get_handler};

void get_req_cb(ArduinoHttpServer::HttpResource resource) {
    for (int i = 0; i < num_get_handlers; ++i)
        get_handlers[i](resource.toString());
}

void render_page(WiFiClient client) {
    String tpl =
        F("HTTP/1.1 200 OK\r\n"          //
          "Content-Type: text/html\r\n"  //
          "\r\n"                         //
          "<!doctype html>\n"            //
          "<html>\n"
          "<head>\n"
          "<meta charset='utf-8'>\n"                               //
          "<meta name='viewport' content='width=device-width'>\n"  //
          // Begin embedded Milligram-style CSS (minimal subset)
          "<style>"
          "*,:after,:before{box-sizing:inherit}"         //
          "html{box-sizing:border-box;font-size:62.5%}"  //
          "body{color:#606c76;font-family:'Roboto','Helvetica "
          "Neue','Helvetica','Arial',sans-serif;"
          "font-size:1.6em;font-weight:300;letter-spacing:.01em;line-height:"
          "1.6}"                                                          //
          "h1,h2,h3,h4,h5,h6{font-weight:300;margin:0 0 2rem 0}"          //
          "h1{font-size:4.6rem}h2{font-size:3.6rem}h3{font-size:2.8rem}"  //
          ".float-right{float:right}"                                     //
          ".container{margin:0 auto;max-width:112rem;padding:0 "
          "2rem;width:100%}"              //
          "fieldset{border:0;padding:0}"  //
          "input,select,textarea{width:100%;height:3.8rem;padding:.6rem 1rem;"
          "border:0.1rem solid #d1d1d1;border-radius:.4rem}"  //
          "input[type='submit']{background-color:#9b4dca;border:none;color:#"
          "fff;"
          "font-weight:700;cursor:pointer}"                              //
          "input[type='submit']:hover{background-color:#606c76}"         //
          "a{color:#9b4dca;text-decoration:none}a:hover{color:#606c76}"  //
          "hr{border:0;border-top:0.1rem solid #f4f5f6;margin:3rem 0}"   //
          "</style>\n"
          "<title>{{title}}</title>\n"  //
          "</head>\n"
          "<body>\n"
          "<div>{{status}}</div>\n"  //
        );

    tpl.replace("{{status}}", String(status));
    tpl.replace("{{title}}", String(UID));

    client.println(tpl);

    render_ui_body(client);

    client.println("</body></html>");
    client.println();
}

WiFiServer server(80);

void ui_setup() {
    server.begin();
    Serial.println("HTTP Server Started Up on port 80");
}

void ui_loop() {
    WiFiClient client(server.available());
    if (client.connected()) {
        ArduinoHttpServer::StreamHttpRequest<1024> httpRequest(client);

        if (httpRequest.readRequest()) {
            ArduinoHttpServer::Method method(
                ArduinoHttpServer::Method::Invalid);
            method = httpRequest.getMethod();

            if (method == ArduinoHttpServer::Method::Get)
                get_req_cb(httpRequest.getResource());
            if (method == ArduinoHttpServer::Method::Post)
                post_req_cb(httpRequest.getResource(), httpRequest.getBody());

            render_page(client);
        } else {
            // HTTP parsing failed. Client did not provide correct HTTP data or
            // client requested an unsupported feature.
            ArduinoHttpServer::StreamHttpErrorReply httpReply(
                client, httpRequest.getContentType());

            const char* pErrorStr(httpRequest.getError().cStr());
            String errorStr(
                pErrorStr);  //! \todo Make HttpReply FixString compatible.

            httpReply.send(errorStr);
        }
        client.stop();
    }
}
#endif
