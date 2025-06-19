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
    String tpl = F(
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "\r\n"
        "<!doctype html>\n"
        "<head>\n"
        "<meta charset='utf-8'>\n"
        "<meta name='viewport' content='width=device-width'>\n"
        // milligram css
        //"<style>*,*:after,*:before{box-sizing:inherit}html{box-sizing:border-box;font-size:62.5%}body{color:#606c76;font-family:'Roboto',
        //'Helvetica Neue', 'Helvetica', 'Arial',
        // sans-serif;font-size:1.6em;font-weight:300;letter-spacing:.01em;line-height:1.6}blockquote{border-left:0.3rem
        // solid
        // #d1d1d1;margin-left:0;margin-right:0;padding:1rem 1.5rem}blockquote
        //*:last-child{margin-bottom:0}.button,button,input[type='button'],input[type='reset'],input[type='submit']{background-color:#9b4dca;border:0.1rem
        // solid
        // #9b4dca;border-radius:.4rem;color:#fff;cursor:pointer;display:inline-block;font-size:1.1rem;font-weight:700;height:3.8rem;letter-spacing:.1rem;line-height:3.8rem;padding:0
        // 3.0rem;text-align:center;text-decoration:none;text-transform:uppercase;white-space:nowrap}.button:focus,.button:hover,button:focus,button:hover,input[type='button']:focus,input[type='button']:hover,input[type='reset']:focus,input[type='reset']:hover,input[type='submit']:focus,input[type='submit']:hover{background-color:#606c76;border-color:#606c76;color:#fff;outline:0}.button[disabled],button[disabled],input[type='button'][disabled],input[type='reset'][disabled],input[type='submit'][disabled]{cursor:default;opacity:.5}.button[disabled]:focus,.button[disabled]:hover,button[disabled]:focus,button[disabled]:hover,input[type='button'][disabled]:focus,input[type='button'][disabled]:hover,input[type='reset'][disabled]:focus,input[type='reset'][disabled]:hover,input[type='submit'][disabled]:focus,input[type='submit'][disabled]:hover{background-color:#9b4dca;border-color:#9b4dca}.button.button-outline,button.button-outline,input[type='button'].button-outline,input[type='reset'].button-outline,input[type='submit'].button-outline{background-color:transparent;color:#9b4dca}.button.button-outline:focus,.button.button-outline:hover,button.button-outline:focus,button.button-outline:hover,input[type='button'].button-outline:focus,input[type='button'].button-outline:hover,input[type='reset'].button-outline:focus,input[type='reset'].button-outline:hover,input[type='submit'].button-outline:focus,input[type='submit'].button-outline:hover{background-color:transparent;border-color:#606c76;color:#606c76}.button.button-outline[disabled]:focus,.button.button-outline[disabled]:hover,button.button-outline[disabled]:focus,button.button-outline[disabled]:hover,input[type='button'].button-outline[disabled]:focus,input[type='button'].button-outline[disabled]:hover,input[type='reset'].button-outline[disabled]:focus,input[type='reset'].button-outline[disabled]:hover,input[type='submit'].button-outline[disabled]:focus,input[type='submit'].button-outline[disabled]:hover{border-color:inherit;color:#9b4dca}.button.button-clear,button.button-clear,input[type='button'].button-clear,input[type='reset'].button-clear,input[type='submit'].button-clear{background-color:transparent;border-color:transparent;color:#9b4dca}.button.button-clear:focus,.button.button-clear:hover,button.button-clear:focus,button.button-clear:hover,input[type='button'].button-clear:focus,input[type='button'].button-clear:hover,input[type='reset'].button-clear:focus,input[type='reset'].button-clear:hover,input[type='submit'].button-clear:focus,input[type='submit'].button-clear:hover{background-color:transparent;border-color:transparent;color:#606c76}.button.button-clear[disabled]:focus,.button.button-clear[disabled]:hover,button.button-clear[disabled]:focus,button.button-clear[disabled]:hover,input[type='button'].button-clear[disabled]:focus,input[type='button'].button-clear[disabled]:hover,input[type='reset'].button-clear[disabled]:focus,input[type='reset'].button-clear[disabled]:hover,input[type='submit'].button-clear[disabled]:focus,input[type='submit'].button-clear[disabled]:hover{color:#9b4dca}code{background:#f4f5f6;border-radius:.4rem;font-size:86%;margin:0
        //.2rem;padding:.2rem
        //.5rem;white-space:nowrap}pre{background:#f4f5f6;border-left:0.3rem
        // solid
        // #9b4dca;overflow-y:hidden}pre>code{border-radius:0;display:block;padding:1rem
        // 1.5rem;white-space:pre}hr{border:0;border-top:0.1rem solid
        // #f4f5f6;margin:3.0rem
        // 0}input[type='color'],input[type='date'],input[type='datetime'],input[type='datetime-local'],input[type='email'],input[type='month'],input[type='number'],input[type='password'],input[type='search'],input[type='tel'],input[type='text'],input[type='url'],input[type='week'],input:not([type]),textarea,select{-webkit-appearance:none;background-color:transparent;border:0.1rem
        // solid
        // #d1d1d1;border-radius:.4rem;box-shadow:none;box-sizing:inherit;height:3.8rem;padding:.6rem
        // 1.0rem
        //.7rem;width:100%}input[type='color']:focus,input[type='date']:focus,input[type='datetime']:focus,input[type='datetime-local']:focus,input[type='email']:focus,input[type='month']:focus,input[type='number']:focus,input[type='password']:focus,input[type='search']:focus,input[type='tel']:focus,input[type='text']:focus,input[type='url']:focus,input[type='week']:focus,input:not([type]):focus,textarea:focus,select:focus{border-color:#9b4dca;outline:0}select{background:url('data:image/svg+xml;utf8,<svg
        // xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 30 8\"
        // width=\"30\"><path fill=\"%23d1d1d1\" d=\"M0,0l6,8l6-8\"/></svg>')
        // center right
        // no-repeat;padding-right:3.0rem}select:focus{background-image:url('data:image/svg+xml;utf8,<svg
        // xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 30 8\"
        // width=\"30\"><path fill=\"%239b4dca\"
        // d=\"M0,0l6,8l6-8\"/></svg>')}select[multiple]{background:none;height:auto}textarea{min-height:6.5rem}label,legend{display:block;font-size:1.6rem;font-weight:700;margin-bottom:.5rem}fieldset{border-width:0;padding:0}input[type='checkbox'],input[type='radio']{display:inline}.label-inline{display:inline-block;font-weight:normal;margin-left:.5rem}.container{margin:0
        // auto;max-width:112.0rem;padding:0 2.0rem;position:relative;width:100%}.row{display:flex;flex-direction:column;padding:0;width:100%}.row.row-no-padding{padding:0}.row.row-no-padding>.column{padding:0}.row.row-wrap{flex-wrap:wrap}.row.row-top{align-items:flex-start}.row.row-bottom{align-items:flex-end}.row.row-center{align-items:center}.row.row-stretch{align-items:stretch}.row.row-baseline{align-items:baseline}.row
        //.column{display:block;flex:1 1
        // auto;margin-left:0;max-width:100%;width:100%}.row
        //.column.column-offset-10{margin-left:10%}.row
        //.column.column-offset-20{margin-left:20%}.row
        //.column.column-offset-25{margin-left:25%}.row
        //.column.column-offset-33,.row
        //.column.column-offset-34{margin-left:33.3333%}.row
        //.column.column-offset-40{margin-left:40%}.row
        //.column.column-offset-50{margin-left:50%}.row
        //.column.column-offset-60{margin-left:60%}.row
        //.column.column-offset-66,.row
        //.column.column-offset-67{margin-left:66.6666%}.row
        //.column.column-offset-75{margin-left:75%}.row
        //.column.column-offset-80{margin-left:80%}.row
        //.column.column-offset-90{margin-left:90%}.row .column.column-10{flex:0
        // 0
        // 10%;max-width:10%}.row .column.column-20{flex:0 0
        // 20%;max-width:20%}.row .column.column-25{flex:0 0
        // 25%;max-width:25%}.row .column.column-33,.row
        // .column.column-34{flex:0 0 33.3333%;max-width:33.3333%}.row
        // .column.column-40{flex:0 0 40%;max-width:40%}.row
        // .column.column-50{flex:0 0 50%;max-width:50%}.row
        // .column.column-60{flex:0 0 60%;max-width:60%}.row
        // .column.column-66,.row .column.column-67{flex:0
        // 0 66.6666%;max-width:66.6666%}.row .column.column-75{flex:0 0
        // 75%;max-width:75%}.row .column.column-80{flex:0 0
        // 80%;max-width:80%}.row .column.column-90{flex:0 0
        // 90%;max-width:90%}.row .column .column-top{align-self:flex-start}.row
        //.column .column-bottom{align-self:flex-end}.row .column
        //.column-center{align-self:center}@media (min-width:
        // 40rem){.row{flex-direction:row;margin-left:-1.0rem;width:calc(100%
        //+ 2.0rem)}.row
        //.column{margin-bottom:inherit;padding:0 1.0rem}}a{color:#9b4dca;text-decoration:none}a:focus,a:hover{color:#606c76}dl,ol,ul{list-style:none;margin-top:0;padding-left:0}dl
        // dl,dl ol,dl ul,ol dl,ol ol,ol ul,ul dl,ul ol,ul
        // ul{font-size:90%;margin:1.5rem 0 1.5rem 3.0rem}ol{list-style:decimal
        // inside}ul{list-style:circle
        // inside}.button,button,dd,dt,li{margin-bottom:1.0rem}fieldset,input,select,textarea{margin-bottom:1.5rem}blockquote,dl,figure,form,ol,p,pre,table,ul{margin-bottom:2.5rem}table{border-spacing:0;display:block;overflow-x:auto;text-align:left;width:100%}td,th{border-bottom:0.1rem
        // solid
        // #e1e1e1;padding:1.2rem 1.5rem}td:first-child,th:first-child{padding-left:0}td:last-child,th:last-child{padding-right:0}@media
        //(min-width:
        // 40rem){table{display:table;overflow-x:initial}}b,strong{font-weight:bold}p{margin-top:0}h1,h2,h3,h4,h5,h6{font-weight:300;letter-spacing:-.1rem;margin-bottom:2.0rem;margin-top:0}h1{font-size:4.6rem;line-height:1.2}h2{font-size:3.6rem;line-height:1.25}h3{font-size:2.8rem;line-height:1.3}h4{font-size:2.2rem;letter-spacing:-.08rem;line-height:1.35}h5{font-size:1.8rem;letter-spacing:-.05rem;line-height:1.5}h6{font-size:1.6rem;letter-spacing:0;line-height:1.4}img{max-width:100%}.clearfix:after{clear:both;content:'
        //';display:table}.float-left{float:left}.float-right{float:right}</style>"
        "<title>{{title}}</title></head><body>"
        "<div>{{status}}</div>");

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
