#include <SFML/Network.hpp>
#include <iostream>
#include <string>
#include <windows.h>
#include <fstream>
#include <sstream>

using namespace std;
HANDLE hConsole;

int main()
{
	string nameBuff;
	string messageBuff;
	// получаем пути до exe криптора и дескриптора
	string encryptName = "encryptE.exe";
	string decryptName = "decryptE.exe";

	string encryptPath;
	string decryptPath;

	char path[260];
	GetModuleFileNameA(NULL, path, 260); // полный путь к exe
	string s = path;
	cout << s << endl;

	// путь к exe файлу обрезаем до \n
	for (int i = s.length(); s[i] != '\\'; i--)
	{
		s.erase(i, 1);
	}
	// к обрезанной строке добавляем название файла, 
	encryptPath = s + encryptName;
	decryptPath = s + decryptName;
	cout << encryptPath << endl << decryptPath << endl;
	//
	sf::TcpSocket socket; // создадим объект типа TCP сокет
	//sf::IpAddress ip = sf::IpAddress::getLocalAddress(); // узнаём наш ip и записываем в переменную для дальнейшего использовая

	cout << "your local IP is: " << sf::IpAddress::getLocalAddress() << endl;
	sf::IpAddress ip;

	ifstream fin("serverip.dat");
	if (fin.is_open())
	{
		fin >> ip;
		fin.close();
		cout << ip << endl;
	}

	char type;

ErrorType:

	cout << "Enter type connecting: [c] - client, [s] - server" << endl;
	cin >> type;

	if (type == 's')
	{
		sf::TcpListener listener; // создаём объект, который будем принимать новых клиентов
		listener.listen(2000); // передаём порт

		if (listener.accept(socket) != sf::Socket::Done) // с помощью функции ассепт мы принимает клиента для обмена данными 
		{
			cout << "Error, client cant join" << endl;
		}
	}
	else if (type == 'c')
	{
		if (socket.connect(ip, 2000) != sf::Socket::Done) // на стороне клиента, мы подключается к серверу через метод коннект, указывая порт и IP сервера
		{
			cout << "Error, cant to connect to servet" << endl;
		}
	}
	else
	{
		cout << "Type Error" << endl;
		goto ErrorType;
	}

	string name;
	cout << "Enter name: ";
	cin >> name;
	cout << endl;

	// записываем имя отправителя в in.dat
	ofstream out;
	out.open("in.dat");
	if (out.is_open())
	{
		out << name << endl;
		out.close();
	}
	// шифруем его, запустив программу шифрования 
	system(encryptPath.c_str());
	// зашифрованное двоичное сообщение записываем в переменную 
	ifstream infile;
	infile.open("message.aes", ios::in | ios::binary);
	if (infile.is_open())
	{
		getline(infile, nameBuff); // The first line of file is the message
		infile.close();
	}



	//

	socket.setBlocking(false); // делаем сокет не блокирующимся, чтобы при передаче данных программа не останавливалась 

	string message = "";
	sf::Packet packet;

	while (true)
	{
		if (GetAsyncKeyState(VK_ESCAPE))
		{
			getline(cin, message);
			// после того, как считали сообщение, мы шифруем его. Помещаем сообщение в in.dat 
			ofstream out;
			out.open("in.dat");
			if (out.is_open())
			{
				out << message << endl;
				out.close();
			}
			// шифруем его, запустив программу шифрования 
			system(encryptPath.c_str());
			// зашифрованное двоичное сообщение записываем в переменную 
			ifstream infile;
			infile.open("message.aes", ios::in | ios::binary);
			if (infile.is_open())
			{
				getline(infile, messageBuff); // The first line of file is the message
				infile.close();
			}
			//
		}

		if (message != "") // Если сообщение не пустое, то мы собираем его в пакет 
		{
			packet.clear(); // очистили
			packet << nameBuff << messageBuff; // собрали
			socket.send(packet); // отправили
			message = "";
		}

		if (socket.receive(packet) == sf::Socket::Done)  // для принятия данных используем метод ресив
		{
			string nameREC;
			string messageREC;

			packet >> nameREC >> messageREC; // извлекаем данные из пакета

			// Расшифровываем два массива, полученные из пакета. Для этого посылаем данные в message.aes
			ofstream outfile;
			outfile.open("message.aes", ios::out | ios::binary);
			if (outfile.is_open())
			{
				outfile << nameREC;
				outfile.close();
			}
			system(decryptPath.c_str()); // затем запускаем программу дешифрования

			ifstream fin("out.dat"); // расшифрованные данные из out.dat в переменную nameREC
			if (fin.is_open()) // считать из файла 
			{
				fin >> nameREC;
				fin.close();
			}
			// аналогично для пересланной переменной messageREC 
			ofstream outfile1;
			outfile.open("message.aes", ios::out | ios::binary);
			if (outfile.is_open())
			{
				outfile << messageREC;
				outfile.close();
			}

			system(decryptPath.c_str());

			ifstream fin1("out.dat");
			if (fin1.is_open())
			{
				fin1 >> messageREC;
				fin1.close();
			}

			for (int i = 0; i < messageREC.length(); i++)
			{
				if (messageREC[i] == '_')
					messageREC[i] = ' ';
			}
			hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(hConsole, 2);

			cout << nameREC << ": " << messageREC << endl; // выводим данные 

			SetConsoleTextAttribute(hConsole, 7);
		}
	}




	return 0;
}
