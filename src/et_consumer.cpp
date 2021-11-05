#include "et.h"
#include <iostream>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h> // for sleep function

#include "FakeDecoder.h"
#include "EventParser.h"
#include "EvioFileReader.h"

using namespace std;


int main(int argc, char* argv[])
{
    //--------------------------------------------------------
    // 1, open et system
    et_sys_id id;
    et_openconfig config;
    et_open_config_init(&config);
    // broadcasting by default
    // et is on an unkown host
    //et_open_config_sethost(config, ET_HOST_ANYWHERE);

    // et is on localhost
    //et_open_config_sethost(config, "192.168.1.188");
    et_open_config_sethost(config, "127.0.0.1");
    // use a direct connection to the ET system
    et_open_config_setcast(config, ET_DIRECT);
    // et system's server is on this port
    et_open_config_setserverport(config, 23911);

    int  open_status = -1;
    //open_status = et_open(&id, "/tmp/et_test_expid_ER2", config);

    while(open_status !=0 )  {
        cout<<"INFO:: Openning ET, please make sure ET is setup..."<<std::endl;
        sleep(5); // pause 5 seconds
        open_status = et_open(&id, "/tmp/et_test_expid_ER2", config);
    }
    cout<<"et open with status: "<<open_status<<std::endl;
    et_open_config_destroy(config);

    //--------------------------------------------------------
    // 2, create station
    et_stat_id stat_id;
    et_statconfig sconfig;
    et_station_config_init(&sconfig);
    et_station_config_setselect(sconfig, ET_STATION_SELECT_ALL);
    et_station_config_setblock(sconfig, ET_STATION_NONBLOCKING);
    et_station_config_setcue(sconfig, 10);
    et_station_config_setuser(sconfig, ET_STATION_USER_MULTI);
    //et_station_config_setrestore(sconfig, ET_STATION_RESTORE_GC);
    et_station_config_setrestore(sconfig, ET_STATION_RESTORE_OUT);
    et_station_config_setprescale(sconfig, 1);

    int station_status = -1;
    while(station_status != 0) {
        cout<<"INFO:: Creating ET station, please make sure you have acess to write disk..."<<endl;
        sleep(5);
        station_status = et_station_create(id, &stat_id, "xinzhan's test et station", sconfig);
    }
    cout<<"et station created: "<<station_status<<endl;
    //et_station_remove(id, stat_id);
    et_station_config_destroy(sconfig);

    //--------------------------------------------------------
    // 3, attach to station
    et_att_id att_id;
    int attach_status = -1;
    while(attach_status != 0) {
        cout<<"INFO:: Attaching ET station, please make sure ET station is created..."<<endl;
        sleep(5);
        attach_status = et_station_attach(id, stat_id, &att_id);
    }
    //et_station_detach(id, att_id);

    //--------------------------------------------------------
    // 4, get event
    et_event *pe;
    struct timespec time;
    // get new event
    //et_event_new(id, att_id, &pe, ET_SLEEP, &time, 1000);
    
    //--------------------------------------------------------
    // fine tuning et
    et_system_setdebug(id, ET_DEBUG_INFO);

    //--------------------------------------------------------
    // fake decoder
    EventParser *parser = new EventParser();
    FakeDecoder *decoder = new FakeDecoder();
    parser -> RegisterRawDecoder(1, decoder);
 
    // get data filled event
    while(et_alive(id))
    {
        //cout<<"getting event in blocking mode: "<<endl;
        //int status = et_event_get(id, att_id, &pe, ET_SLEEP, &time); // blocking
        cout<<"getting event in nonblocking mode: "<<endl;
        int status = et_event_get(id, att_id, &pe, ET_ASYNC, &time); // non-blocking 
        cout<<"et_event_get with status: "<<status<<endl;

        if(status != ET_OK) continue;

        cout<<"event length: "<<pe->length<<endl;
        int event_length = (int)pe->length;

        // version 1:)
        // with memory leak 
        //void *destination_buf = (void*)malloc(event_length*4);
        //void **tmp = &destination_buf;

        // version 2:)
        // without memory leak
        char buf = 'c';
        void *des_buf = (void*)&buf;
        void **tmp = &des_buf;

        if(tmp == NULL) cout<<"destination buffer is NULL."<<endl;
        if(pe->pdata == NULL) cout<<"source buffer is NULL."<<endl;
        status = et_event_getdata(pe, tmp); // this function requires that tmp != NULL, so version 1 or version 2 must be used
        cout<<"got 1 event, with status: "<<status<<endl;

        // print event buffer
        //for(int i=0;i<event_length;i++) {
        //    cout<<hex<<"0x"<<((int*)(*tmp))[i]<<endl;
        //}
        //cout<<dec;

        // switch endianess, need to switch from internet format to local format
        for(int i=0; i<event_length/4; i++) {
            ((uint32_t*)(*tmp))[i] = ntohl(((uint32_t*)(*tmp))[i]);
        }

        // test event parser
        cout<<"parsing event..."<<endl;
        // need to shift by 8 uint32_t words, to skip coda event header, decoder won't handle this header
        parser -> ParseEvent((uint32_t*)(*tmp) + 8, event_length/4);

        // put event back
        et_event_put(id, att_id, pe);

        sleep(5);
    }
    // dump event
    //et_event_dump(id, att_id, pe);

    cout<<"killing et..."<<endl;
    et_kill(id);
    cout<<"et killed..."<<endl;

    cout<<"press to exit..."<<endl;
    getchar();

    return 0;
}



/*
int main(int argc, char* argv[])
{
    EvioFileReader *file_reader = new EvioFileReader();
    file_reader -> SetFile("/home/daq/apps/coda/data//et_test2_112.evio.0");
    file_reader -> OpenFile();

    EventParser *event_parser = new EventParser();
    FakeDecoder *decoder = new FakeDecoder();

    event_parser -> RegisterRawDecoder(1, decoder);

    const uint32_t *pBuf;
    uint32_t pBufLen;

    if(file_reader -> ReadNoCopy(&pBuf, &pBufLen) != S_SUCCESS)
        cout<<"failed ..."<<endl;

    int ev_index = 0;
    while(file_reader -> ReadNoCopy(&pBuf, &pBufLen) == S_SUCCESS)
    {
        event_parser -> ParseEvent(pBuf, pBufLen);
        ev_index++;
        cout<<"event number: "<<ev_index<<endl;
        getchar();
    }

    return 0;
}
*/
