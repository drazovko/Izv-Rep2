#include<iostream>
#include<string>
#include<Poco/Net/DatagramSocket.h>
#include<Poco/Net/SocketAddress.h>
#include<Poco/Net/Socket.h>
#include<Poco/ByteOrder.h>
#include<random>
#include<chrono>
#include<sstream>
#include<ifaddrs.h>
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
    struct PorukaBr9{
        uint8_t tipPoruke;
        uint64_t identifikatorStrujanja;
    } porukaBr9;
    PorukaMajstor() { }
    ~PorukaMajstor() { }
    string Ping(){
        poruka.clear();
        uint8_t tipPoruke = 1;
        poruka.append(to_string(tipPoruke));
        poruka.append("Ovo je ping poruka");
        return poruka;
    }
    u_char* FIND_STREAM_SOURCE(u_int64_t izbornik){
        cout << "\n\tSlažem poruku MSG_FIND_STREAM_SOURCE: " << endl << endl;
        porukaBr9.tipPoruke = MSG_FIND_STREAM_SOURCE;
        uint64_t klasaC = 0x4000000000000000;
        switch(izbornik){
            case 1:
                porukaBr9.identifikatorStrujanja = klasaC;
                break;
            case 2:
                IdentifikatorStrujanja();
                cout << "Identifikator str. = " << identifikatorStrujanja
                 << hex << ", hex = " << identifikatorStrujanja << dec << endl;
                porukaBr9.identifikatorStrujanja = klasaC | identifikatorStrujanja;
                break;
            default:
                porukaBr9.identifikatorStrujanja = izbornik;
        }
        cout << "Identifikator str. + klC = " << porukaBr9.identifikatorStrujanja
             << ", hex = " << hex << porukaBr9.identifikatorStrujanja 
             << dec << endl;
        porukaBr9.identifikatorStrujanja = byteOrderMoj.toNetwork(porukaBr9.identifikatorStrujanja);  
        cout << "\tU mrežnom obliku: " << endl;
        cout << "Identifikator str. + klC = " << porukaBr9.identifikatorStrujanja
             << ", hex = " << hex << porukaBr9.identifikatorStrujanja 
             << dec << endl;
        u_char* A;
        A = (u_char*)&porukaBr9;
        PrikazPorukePoBajtovima(A, 16);
        return A;
    }
    u_char* STREAM_REMOVE(bool fiksniIdentifikator = false){
        cout << "\n\tSlazem poruku MSG_STREAM_REMOVE: " << endl << endl;
        porukaBr9.tipPoruke = MSG_STREAM_REMOVE;
        uint64_t klasaC = 0x4000000000000000;
        if(fiksniIdentifikator == false){
            porukaBr9.identifikatorStrujanja = porukaBr4.identifikatorStrujanja;
            cout << "Identifikator str. = " << porukaBr9.identifikatorStrujanja
             << hex << ", hex = " << porukaBr9.identifikatorStrujanja << dec << endl;
        }else{
            porukaBr9.identifikatorStrujanja = klasaC;
            cout << "Identifikator str. je klasa C(fiksni identifikator) = " << porukaBr9.identifikatorStrujanja
             << hex << ", hex = " << porukaBr9.identifikatorStrujanja << dec << endl;
            porukaBr9.identifikatorStrujanja = byteOrderMoj.toNetwork(porukaBr9.identifikatorStrujanja);    
        }
        u_char* A;
        A = (u_char*)&porukaBr9;
        u_char B;
        int BB;
        cout << "\nPrikaz poruke po bajtovima u hexu: " << endl;
        for(int i = 1; i<=16; i++){
            cout << i << " ";
        }
        cout << endl << hex;
        for(int i = 0; i<16; i++){
            BB = (int)A[i];
            cout << BB << " ";
        }
        cout << endl << dec;
        return A;
    }

    u_char* STREAM_ADVERTISEMENT2(bool fiksniIdentifikator = false) {
        cout << "\n\tSlazem poruku MSG_STREAM_ADVERTISEMENT: " << endl << endl;
        porukaBr4.tipPoruke = MSG_STREAM_ADVERTISEMENT;;
        uint64_t klasaC = 0x4000000000000000;
        cout << "Klasa C = " << klasaC << ", hex = " << hex << klasaC 
             << dec << endl;
        if(fiksniIdentifikator == false){
            IdentifikatorStrujanja();
            cout << "Identifikator str. = " << identifikatorStrujanja
                 << hex << ", hex = " << identifikatorStrujanja << dec << endl;
            porukaBr4.identifikatorStrujanja = klasaC | identifikatorStrujanja;
        }else{
            porukaBr4.identifikatorStrujanja = klasaC | 7;
            cout << "Identifikator str. = " << identifikatorStrujanja
                 << hex << ", hex = " << identifikatorStrujanja << dec << endl;
        }
        
        cout << "Identifikator str. + klC = " << porukaBr4.identifikatorStrujanja
             << ", hex = " << hex << porukaBr4.identifikatorStrujanja 
             << dec << endl;
        //slaganje bajtova u mrežni oblik
        porukaBr4.identifikatorStrujanja = byteOrderMoj.toNetwork(porukaBr4.identifikatorStrujanja);
        porukaBr4.adresaIzvora.tipArdese = 0;
        porukaBr4.adresaIzvora.IPAdresa = 0;
        porukaBr4.adresaIzvora.tipArdese = 1;
        int n = inet_pton(AF_INET, "192.168.5.101", &porukaBr4.adresaIzvora.IPAdresa);
        porukaBr4.adresaIzvora.port = 12000;
        porukaBr4.adresaIzvora.port = byteOrderMoj.toNetwork(porukaBr4.adresaIzvora.port);
        
        cout << "\n\tSlozena poruka u mreznom obliku: " << endl;
        cout << "Tip poruke:\t\t" << (int)porukaBr4.tipPoruke << endl;
        cout << "Identif.strujanja:\t" << porukaBr4.identifikatorStrujanja 
            << ", hex: " << hex << porukaBr4.identifikatorStrujanja << dec << endl;
        cout << "Tip lokalne sdrese:\t" << (int)porukaBr4.adresaIzvora.tipArdese << endl;
        cout << "Lokalna IP adresa:\t" << porukaBr4.adresaIzvora.IPAdresa 
            << ", hex: " << hex << porukaBr4.adresaIzvora.IPAdresa << dec << endl;
        cout << "Lokalni broj porta:\t" << porukaBr4.adresaIzvora.port
             << ", hex: " << hex << porukaBr4.adresaIzvora.port << dec << endl << endl;

        u_char* A;
        A = (u_char*)&porukaBr4;
        u_char B;
        int BB;
        cout << "\nPrikaz poruke po bajtovima u hexu: " << endl;
        for(int i = 1; i<=30; i++){
            cout << i << " ";
        }
        cout << endl << hex;
        for(int i = 0; i<30; i++){
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
    void PrikazPorukePoBajtovima(u_char* A, int brojBajtova){
        u_char B;
        int BB;
        cout << "\nPrikaz poruke po bajtovima u hexu: " << endl;
        for(int i = 1; i<=brojBajtova; i++){
            cout << i << " ";
        }
        cout << endl << hex;
        for(int i = 0; i<brojBajtova; i++){
            BB = (int)A[i];
            cout << BB << " ";
        }
        cout << endl << dec;
    }
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

string getIPAddres(){
    string ipAddress = "Unable to get IP Address";
    struct ifaddrs* interfaces = nullptr;
    struct ifaddrs* temp_addr = nullptr;
    int success = 0;
    //retrive the current interfaces - returns 0 on success
    success = getifaddrs(&interfaces);
    if(success == 0){
        //Loop through linked list of interfaces
        temp_addr = interfaces;
        while(temp_addr != nullptr){
            if(temp_addr->ifa_addr->sa_family == AF_INET){
                cout << temp_addr->ifa_name << endl
                 << "AF_INET" << endl
                 << inet_ntoa(((struct sockaddr_in*)temp_addr->ifa_addr)->sin_addr) << endl;
                // Check if interface is en0 which is the wifi connection 
                if(strcmp(temp_addr->ifa_name, "ens33") == 0){
                    ipAddress = inet_ntoa(((struct sockaddr_in*)temp_addr->ifa_addr)->sin_addr);
                }
            } else {
                if(temp_addr->ifa_addr->sa_family == AF_INET6){
                    cout << temp_addr->ifa_name << endl
                         << "AF_INET6" << endl
                         << inet_ntoa(((struct sockaddr_in*)temp_addr->ifa_addr)->sin_addr) << endl;
                } else{
                    cout << temp_addr->ifa_name << endl
                         << temp_addr->ifa_addr->sa_family << endl
                         << inet_ntoa(((struct sockaddr_in*)temp_addr->ifa_addr)->sin_addr) << endl;
                }
            }
            cout << endl;
            temp_addr = temp_addr->ifa_next;
        }
    }
    // Free memory
    freeifaddrs(interfaces);
    return ipAddress;
}


int main(){
    int novaKomanda;
    PorukaMajstor porukaMajstor;
    string poruka;

    cout << getIPAddres() << endl << endl;
    string IPAdresaPosluziteljaZaPronalazenje = "192.168.5.100";

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
    int brojKomande = 4;
    do{
//        if(brojKomande == 4){
            cout << "MSG_PING =             1\n"
                << "MSG_PONG =             2\n"
                << "MSG_PONG_REG_REQ =     3\n" 
                << "MSG_STREAM_ADVERTISEMENT = 4\n"
                << "Slanje fiksnog ident.br. = 44\n"
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
                << "MSG_REGISTER_FORWARDING = 19\n" 
                << "Kraj programa =         0\n" << endl;
//        }
        cout << "Unesite broj komande: ";
        cin >> brojKomande;
        u_char* A;
        char ponovoPoslati = 'n';
        switch (brojKomande)
        {
            case 44:
                cout << "\tSlanje fiksnog identifikacijskog broja" << endl;
            case imePoruke::MSG_STREAM_ADVERTISEMENT:
                
                if(brojKomande == 44){
                    A = porukaMajstor.STREAM_ADVERTISEMENT2(true);
                }else{
                    A = porukaMajstor.STREAM_ADVERTISEMENT2();
                }
                do
                {
                    int n = ds.sendTo(A, 28, socAddrPosluziteljaZaPronalazenje);
                    cout << "\n\tPoruka poslana serveru!!!" << endl << endl;
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
                        cout << "   Odgovor u mreznom obliku:" << endl << endl;
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
                        struct IdentifierNotUsableStruktura {
                            u_int8_t tipPoruke;
                            u_int64_t identifikatorStrujanja;
                            uint8_t tipJavneAdrese;
                            uint32_t javnaIPAdresa;
                            uint16_t javniBrojPorta;
                        } dolaznaPorukaIdentifierNotUsable;
                        IdentifierNotUsableStruktura* pokIdentifierNotUsableStruktura;
                        pokIdentifierNotUsableStruktura =(IdentifierNotUsableStruktura*)&poljeZaPrijem[0];
                        Poco::ByteOrder byteOrdermoj2;
                        char polje[1024];
                        string string3;
                        switch (dolaznaPorukaStreamRegistred.tipPoruke)
                        {
                            case imePoruke::MSG_STREAM_REGISTERED:
                                cout << "Tip poruke:\t\t" << (int)dolaznaPorukaStreamRegistred.tipPoruke
                                     << endl;
                                cout << "Identif.strujanja:\t" << dolaznaPorukaStreamRegistred.identifikatorStrujanja
                                     << ", hex: " << hex << dolaznaPorukaStreamRegistred.identifikatorStrujanja << dec << endl;
                                cout << "TTL u sekund:\t\t" << dolaznaPorukaStreamRegistred.TTL_u_sekundama
                                     << endl;
                                cout << "Tip javne adrese:\t" << (int)dolaznaPorukaStreamRegistred.tipJavneAdrese
                                     << endl;
                                string3 = inet_ntop(AF_INET, &dolaznaPorukaStreamRegistred.javnaIPAdresa, polje, INET_ADDRSTRLEN);
                                cout << "Javna IP adresa:\t" << dolaznaPorukaStreamRegistred.javnaIPAdresa
                                     << ", hex: " << hex << dolaznaPorukaStreamRegistred.javnaIPAdresa << dec 
                                     << ", " << string3 << endl;
                                cout << "Broj porta:\t\t" << dolaznaPorukaStreamRegistred.javniBrojPorta
                                     << ", hex: " << hex << dolaznaPorukaStreamRegistred.javniBrojPorta << dec
                                     << ", " << byteOrdermoj2.fromNetwork(dolaznaPorukaStreamRegistred.javniBrojPorta)
                                     << endl << endl;
                                break;
                            case imePoruke::MSG_IDENTIFIER_NOT_USABLE:
                                dolaznaPorukaIdentifierNotUsable = *pokIdentifierNotUsableStruktura;
                                cout << "Broj i tip poruke:\t" << (int)dolaznaPorukaIdentifierNotUsable.tipPoruke
                                     << ", MSG_IDENTIFIER_NOT_USABLE" << endl;
                                cout << "Identif.strujanja:\t" << dolaznaPorukaIdentifierNotUsable.identifikatorStrujanja
                                     << ", hex: " << hex << dolaznaPorukaIdentifierNotUsable.identifikatorStrujanja << dec << endl;
                                cout << "Tip javne adrese:\t" << (int)dolaznaPorukaIdentifierNotUsable.tipJavneAdrese
                                     << endl;
                                string3 = inet_ntop(AF_INET, &dolaznaPorukaIdentifierNotUsable.javnaIPAdresa, polje, INET_ADDRSTRLEN);
                                cout << "Javna IP adresa:\t" << dolaznaPorukaIdentifierNotUsable.javnaIPAdresa
                                     << ", hex: " << hex << dolaznaPorukaIdentifierNotUsable.javnaIPAdresa << dec 
                                     << ", " << string3 << endl;
                                cout << "Broj porta:\t\t" << dolaznaPorukaIdentifierNotUsable.javniBrojPorta
                                     << ", hex: " << hex << dolaznaPorukaIdentifierNotUsable.javniBrojPorta << dec
                                     << ", " << byteOrdermoj2.fromNetwork(dolaznaPorukaIdentifierNotUsable.javniBrojPorta)
                                     << endl << endl;
                                break; 
                            default:
                                cout << "Primljena je poruka nepoznatog tipa, broj poruke je "
                                     << (int)dolaznaPorukaStreamRegistred.tipPoruke << endl;
                                break;
                        }
                    }
                    cout << "\nPonovo poslati isti identifikator strujanja?(d/n) ";
                    cin >> ponovoPoslati;
                } while (ponovoPoslati == 'd');
                cout << "Poslati poruku MSG_STREAM_REMOVE sa istim identifikatorom strujanja?(d/n)" << endl;
                cin >> ponovoPoslati;
                if(ponovoPoslati == 'd'){
                    A = porukaMajstor.STREAM_REMOVE();
                    while (ponovoPoslati == 'd'){
                        int n = ds.sendTo(A, 28, socAddrPosluziteljaZaPronalazenje);
                        cout << "\n\tPoruka poslana serveru!!!" << endl << endl;
                        cout << "\nPonovo poslati isti identifikator strujanja sa porukom MSG_STREAM_REMOVE?(d/n) ";
                        cin >> ponovoPoslati;
                    }
                }
                break;
            case imePoruke::MSG_STREAM_REMOVE:
                A = porukaMajstor.STREAM_REMOVE(true);
                do{
                    int n = ds.sendTo(A, 16, socAddrPosluziteljaZaPronalazenje);
                    cout << "\n\tPoruka poslana serveru!!!" << endl << endl;
                    cout << "\nPonovo poslati FIKSNI identifikator strujanja sa porukom MSG_STREAM_REMOVE?(d/n) ";
                    cin >> ponovoPoslati;
                } while (ponovoPoslati == 'd');
                break;
            case imePoruke::MSG_FIND_STREAM_SOURCE:
                do{
                    cout << "\n\tUpiši koji identifikator strujanja želiš: " << endl << endl;
                    cout << "1. Fiksni identifikator strujanja" << endl;
                    cout << "2. Slučajni identifikator strujanja" << endl;
                    cout << "3. Upis identifikatora strujanja: " << endl << endl;
                    u_int64_t izbornik;
                    uint64_t klasaC =    0x4000000000000000;
                    uint64_t klasaCMax = 0x40FFFFFFFFFFFFFF;
                    cin >> izbornik;
                    uint64_t identifikatorStrujanja;
                    switch(izbornik){
                        case 1:
                            A = porukaMajstor.FIND_STREAM_SOURCE(1);
                            break;
                        case 2:
                            A = porukaMajstor.FIND_STREAM_SOURCE(2);
                            break;
                        case 3:
                            do{
                                cout << "Upiši identifikator strujanja: " << endl << "Min = " << klasaC
                                     << ", hex = " << hex << klasaC << dec << endl << "Max = " << klasaCMax
                                     << ", hex = " << hex << klasaCMax << dec << endl << "______";
                                cin >> identifikatorStrujanja;
                            }while(identifikatorStrujanja < klasaC || identifikatorStrujanja > klasaCMax);
                            A = porukaMajstor.FIND_STREAM_SOURCE(identifikatorStrujanja);
                            break;
                        default:
                            "Nepostojeći izbor!";
                            ponovoPoslati = 'd';
                    }
                }while(ponovoPoslati == 'd');
                do{
                    int n = ds.sendTo(A, 16, socAddrPosluziteljaZaPronalazenje);
                    cout << "\n\tPoruka poslana serveru!!!" << endl << endl;
                    cout << "\nPonovo poslati poruku MSG_FIND_STREAM_SOURCE?(d/n) ";
                    cin >> ponovoPoslati;
                } while (ponovoPoslati == 'd');
                break;
            case 0:
                cout << "Kraj programa!!!!" << endl << endl;
                break;
            case MSG_PONG:
                do{
                    cout << "Pong :-)" << endl;
                    cout << "\tNije implementirano" << endl << endl;
                    cout << "Ponovo poslati pong? (d/n) ";
                    cin >> ponovoPoslati;
                }while(ponovoPoslati == 'd');
                break;
            default:
                cout << "\tNije implementirano" << endl << endl;
                break;
        } 
        novaKomanda = brojKomande;
    }while(novaKomanda != 0);
    
    return 0;
}