#include<iostream>
#include<string>
#include<Poco/Net/DatagramSocket.h>
#include<Poco/Net/SocketAddress.h>
#include<Poco/Net/Socket.h>
#include<Poco/ByteOrder.h>
#include<random>
#include<chrono>
#include<sstream>
using namespace std;
using namespace std::chrono;
using namespace Poco::Net;

enum imePoruke {
    MSG_PING = 1,
    MSG_PONG,
    MSG_PONG_REG_REQ,
    MSG_STREAM_ADVERTISEMENT,
    MSG_STREAM_REGISTERED,
    MSG_IDENTIFIER_NOT_USABLE,
    MSG_FIND_STREAM_SOURCE,
    MSG_STREAM_SOURCE_DATA,
    MSG_STREAM_REMOVE,
    MSG_MULTIMEDIA,
    MSG_REQUEST_STREAMING,
    MSG_FORWARD_PLAYER_READY,
    MSG_PLAYER_READY,
    MSG_SOURCE_READY,
    MSG_REQ_RELAY_LIST,
    MSG_RELAY_LIST,
    MSG_SHUTTING_DOWN,
    MSG_PLEASE_FORWARD,
    MSG_REGISTER_FORWARDING 
};

class PorukaMajstor
{
private:
    string poruka;
    uint64_t identifikatorStrujanja;
    Poco::ByteOrder byteOrderMoj;
    
    struct AdresaIzvora
    {
        uint8_t tipArdese;
        uint32_t IPAdresa;
        uint16_t port;
    } adresaIzvoraGlavna;
    
public:
    struct PorukaSaDuzinom{
        char poljeZaPoruku[1024];
        size_t duzinaPoruke;
    } porukaSaDuzinom;
    struct PorukaBr4{
        uint8_t tipPoruke;
        uint64_t identifikatorStrujanja;
        AdresaIzvora adresaIzvora;
    } porukaBr4;
    PorukaMajstor() { }
    ~PorukaMajstor() { }
    string Ping(){
        poruka.clear();
        uint8_t tipPoruke = 1;
        poruka.append(to_string(tipPoruke));
        poruka.append("Ovo je ping poruka");
        return poruka;
    }
    u_char* STREAM_ADVERTISEMENT2() {
        porukaBr4.tipPoruke = MSG_STREAM_ADVERTISEMENT;;
        uint64_t klasaC = 0x4000000000000000;
        cout << "Klasa C = " << klasaC << ", hex = " << hex << klasaC 
             << dec << endl;
        IdentifikatorStrujanja();
        cout << "Identifikator str. = " << identifikatorStrujanja
             << hex << ", hex = " << identifikatorStrujanja << dec << endl;
        porukaBr4.identifikatorStrujanja = klasaC | identifikatorStrujanja;
        cout << "Identifikator str. + klC = " << porukaBr4.identifikatorStrujanja
             << ", hex = " << hex << porukaBr4.identifikatorStrujanja 
             << dec << endl;
        porukaBr4.identifikatorStrujanja = byteOrderMoj.toNetwork(porukaBr4.identifikatorStrujanja);
        porukaBr4.adresaIzvora.tipArdese = 0;
        porukaBr4.adresaIzvora.IPAdresa = 0;
        porukaBr4.adresaIzvora.tipArdese = 1;
        int n = inet_pton(AF_INET, "192.168.5.101", &porukaBr4.adresaIzvora.IPAdresa);
        porukaBr4.adresaIzvora.port = 12000;
        porukaBr4.adresaIzvora.port = byteOrderMoj.toNetwork(porukaBr4.adresaIzvora.port);
        
        u_char* A;
        A = (u_char*)&porukaBr4;
        u_char B;
        int BB;
        cout << "Hex: " << endl;
        for(int i = 1; i<=32; i++){
            cout << i << " ";
        }
        cout << endl << hex;
        for(int i = 0; i<32; i++){
            BB = (int)A[i];
            cout << BB << " ";
        }
        cout << endl << dec;
        return A;
    }

    string STREAM_ADVERTISEMENT() {
        poruka.clear();
        uint8_t tipPoruke = MSG_STREAM_ADVERTISEMENT;
        poruka.append(to_string(tipPoruke));
        uint64_t klasaC = 0x4000000000000000;
        cout << "Klasa C = " << klasaC << ", hex = " << hex << klasaC << endl;

        IdentifikatorStrujanja();
        cout << "Identifikator strujanja = " << dec << identifikatorStrujanja
             << ", hex = " << hex << identifikatorStrujanja << endl;
        identifikatorStrujanja = identifikatorStrujanja | klasaC;

        cout << "Identifikator strujanja i klasa C = " << dec << identifikatorStrujanja
             << ", hex = " << hex << identifikatorStrujanja << endl;
        
        uint64_t testidentifikator = byteOrderMoj.toBigEndian(identifikatorStrujanja);
        testidentifikator = byteOrderMoj.toLittleEndian(identifikatorStrujanja);
        testidentifikator = byteOrderMoj.toNetwork(identifikatorStrujanja);
        
        poruka.append(to_string(testidentifikator));
        return poruka;
    }

private:
    uint64_t& IdentifikatorStrujanja(){
        int najmanji{1};
        int najveci{1000000};
        auto klica = system_clock::now().time_since_epoch().count();
        default_random_engine generator (static_cast<unsigned>(klica));
        uniform_int_distribution<uint64_t> raspodjela(najmanji, najveci);
        identifikatorStrujanja = raspodjela(generator);
        return identifikatorStrujanja;
    }
};


int main(){
    char novaKomanda;
    PorukaMajstor porukaMajstor;
    string poruka;
    string IPAdresaPosluziteljaZaPronalazenje = "192.168.1.21";

    SocketAddress sa("0.0.0.0", 12000);
    DatagramSocket ds(sa);
    SocketAddress socAddrPosluziteljaZaPronalazenje(
        IPAdresaPosluziteljaZaPronalazenje, 12000);
    
    const int vrijemeCekanjaUSecReceiveFrom = 3;
    const int vrijemeCekanjaUMiliSecReceiveFrom = 0;
    Poco::Timespan timeSpanZaPrijem;
    timeSpanZaPrijem.assign(vrijemeCekanjaUSecReceiveFrom, vrijemeCekanjaUMiliSecReceiveFrom);
    ds.setReceiveTimeout(timeSpanZaPrijem); 
    SocketAddress posiljatelj;
    u_char poljeZaPrijem[1024];
    do{
        cout << "Unesite broj za komandu: \n" << endl;
        cout << "MSG_PING =             1\n"
            << "MSG_PONG =             2\n"
            << "MSG_PONG_REG_REQ =     3\n" 
            << "MSG_STREAM_ADVERTISEMENT = 4\n"
            << "MSG_STREAM_REGISTERED = 5\n"
            << "MSG_IDENTIFIER_NOT_USABLE = 6\n" 
            << "MSG_FIND_STREAM_SOURCE = 7\n"
            << "MSG_STREAM_SOURCE_DATA = 8\n" 
            << "MSG_STREAM_REMOVE =    9\n" 
            << "MSG_MULTIMEDIA =       10\n" 
            << "MSG_REQUEST_STREAMING = 11\n" 
            << "MSG_FORWARD_PLAYER_READY = 12\n"
            << "MSG_PLAYER_READY =     13\n"
            << "MSG_SOURCE_READY =     14\n"
            << "MSG_REQ_RELAY_LIST =   15\n"
            << "MSG_RELAY_LIST =       16\n"
            << "MSG_SHUTTING_DOWN =    17\n"
            << "MSG_PLEASE_FORWARD =   18\n"
            << "MSG_REGISTER_FORWARDING = 19\n" << endl;
        cout << "Unesite broj komande: ";
        int brojKomande;
        cin >> brojKomande;
        u_char* A;
        switch (brojKomande)
        {
            case imePoruke::MSG_STREAM_ADVERTISEMENT:
                char ponovoPoslati;
                A = porukaMajstor.STREAM_ADVERTISEMENT2();
                do
                {
                    cout << "\tMSG_STREAM_ADVERTISEMENT\n\n";
                    cout << " Ovo su moji podaci: "<< sa.toString() 
                         << "\n" << endl;
                    int n = ds.sendTo(A, 28, socAddrPosluziteljaZaPronalazenje);
                    
                    cout << "---------------Cekam odgovor 3 sekunde---------" << endl << endl;
                    bool ispisParametara = true;
                    try
                    {
                        ds.receiveFrom(poljeZaPrijem, sizeof(poljeZaPrijem), posiljatelj);    
                    }
                    catch(const std::exception& e)
                    {
                        std::cerr << e.what() << '\n';
                        cout << "   Nije stigao odgovor unutar 3 sekunde" << endl << endl;
                        ispisParametara = false;
                    }
                    if(ispisParametara){
                        cout << "   Odgovor:" << endl;
                        struct StreamRegistredStruktura {
                            u_int8_t tipPoruke;
                            u_int64_t identifikatorStrujanja;
                            u_int16_t TTL_u_sekundama;
                            uint8_t tipJavneAdrese;
                            uint32_t javnaIPAdresa;
                            uint16_t javniBrojPorta;
                        } dolaznaPorukaStreamRegistred;
                        StreamRegistredStruktura* pokStreamRegistredStruktura;
                        pokStreamRegistredStruktura = (StreamRegistredStruktura*)&poljeZaPrijem[0];
                        dolaznaPorukaStreamRegistred = *pokStreamRegistredStruktura;
                        switch (dolaznaPorukaStreamRegistred.tipPoruke)
                        {
                            case imePoruke::MSG_STREAM_REGISTERED:
                                cout << "Tip poruke: " << (int)dolaznaPorukaStreamRegistred.tipPoruke
                                     << endl;
                                cout << "Identif.strujanja:\t" << dolaznaPorukaStreamRegistred.identifikatorStrujanja
                                     << endl;
                                cout << "TTL u sekund:\t" << dolaznaPorukaStreamRegistred.TTL_u_sekundama
                                     << endl;
                                cout << "Tip javne adrese:\t" << (int)dolaznaPorukaStreamRegistred.tipJavneAdrese
                                     << endl;
                                cout << "Javna IP adresa:\t" << dolaznaPorukaStreamRegistred.javnaIPAdresa
                                     << endl;
                                cout << "Broj porta:\t" << dolaznaPorukaStreamRegistred.javniBrojPorta
                                     << endl << endl;
                                break;
                        
                            default:
                                cout << "Primljena je poruka nepoznatog tipa: "
                                     << dolaznaPorukaStreamRegistred.tipPoruke << endl;
                                break;
                        }
                    }
                    cout << "\nPonovo poslati isti identifikator strujanja?(d/n) ";
                    cin >> ponovoPoslati;
                } while (ponovoPoslati == 'd');
                
                
                break;
            case MSG_PONG:
                cout << "Pong" << endl;
                //    break;
    
            default:
                cout << "\tNije implementirano" << endl << endl;
                break;
        } 

        cout << "\nNova komanda?(d/n) ";
        cin >> novaKomanda;
    }while(novaKomanda == 'd');
    
    

    
/*
    char ooo;
    cout << "\nUnesite znak za kraj: ";
    cin >> ooo;
*/    return 0;
}