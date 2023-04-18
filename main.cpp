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
	// �������� ���� �� exe �������� � �����������
	string encryptName = "encryptE.exe";
	string decryptName = "decryptE.exe";

	string encryptPath;
	string decryptPath;

	char path[260];
	GetModuleFileNameA(NULL, path, 260); // ������ ���� � exe
	string s = path;
	cout << s << endl;

	// ���� � exe ����� �������� �� \n
	for (int i = s.length(); s[i] != '\\'; i--)
	{
		s.erase(i, 1);
	}
	// � ���������� ������ ��������� �������� �����, 
	encryptPath = s + encryptName;
	decryptPath = s + decryptName;
	cout << encryptPath << endl << decryptPath << endl;
	//
	sf::TcpSocket socket; // �������� ������ ���� TCP �����
	//sf::IpAddress ip = sf::IpAddress::getLocalAddress(); // ����� ��� ip � ���������� � ���������� ��� ����������� �����������

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
		sf::TcpListener listener; // ������ ������, ������� ����� ��������� ����� ��������
		listener.listen(2000); // ������� ����

		if (listener.accept(socket) != sf::Socket::Done) // � ������� ������� ������ �� ��������� ������� ��� ������ ������� 
		{
			cout << "Error, client cant join" << endl;
		}
	}
	else if (type == 'c')
	{
		if (socket.connect(ip, 2000) != sf::Socket::Done) // �� ������� �������, �� ������������ � ������� ����� ����� �������, �������� ���� � IP �������
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

	// ���������� ��� ����������� � in.dat
	ofstream out;
	out.open("in.dat");
	if (out.is_open())
	{
		out << name << endl;
		out.close();
	}
	// ������� ���, �������� ��������� ���������� 
	system(encryptPath.c_str());
	// ������������� �������� ��������� ���������� � ���������� 
	ifstream infile;
	infile.open("message.aes", ios::in | ios::binary);
	if (infile.is_open())
	{
		getline(infile, nameBuff); // The first line of file is the message
		infile.close();
	}



	//

	socket.setBlocking(false); // ������ ����� �� �������������, ����� ��� �������� ������ ��������� �� ��������������� 

	string message = "";
	sf::Packet packet;

	while (true)
	{
		if (GetAsyncKeyState(VK_ESCAPE))
		{
			getline(cin, message);
			// ����� ����, ��� ������� ���������, �� ������� ���. �������� ��������� � in.dat 
			ofstream out;
			out.open("in.dat");
			if (out.is_open())
			{
				out << message << endl;
				out.close();
			}
			// ������� ���, �������� ��������� ���������� 
			system(encryptPath.c_str());
			// ������������� �������� ��������� ���������� � ���������� 
			ifstream infile;
			infile.open("message.aes", ios::in | ios::binary);
			if (infile.is_open())
			{
				getline(infile, messageBuff); // The first line of file is the message
				infile.close();
			}
			//
		}

		if (message != "") // ���� ��������� �� ������, �� �� �������� ��� � ����� 
		{
			packet.clear(); // ��������
			packet << nameBuff << messageBuff; // �������
			socket.send(packet); // ���������
			message = "";
		}

		if (socket.receive(packet) == sf::Socket::Done)  // ��� �������� ������ ���������� ����� �����
		{
			string nameREC;
			string messageREC;

			packet >> nameREC >> messageREC; // ��������� ������ �� ������

			// �������������� ��� �������, ���������� �� ������. ��� ����� �������� ������ � message.aes
			ofstream outfile;
			outfile.open("message.aes", ios::out | ios::binary);
			if (outfile.is_open())
			{
				outfile << nameREC;
				outfile.close();
			}
			system(decryptPath.c_str()); // ����� ��������� ��������� ������������

			ifstream fin("out.dat"); // �������������� ������ �� out.dat � ���������� nameREC
			if (fin.is_open()) // ������� �� ����� 
			{
				fin >> nameREC;
				fin.close();
			}
			// ���������� ��� ����������� ���������� messageREC 
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

			cout << nameREC << ": " << messageREC << endl; // ������� ������ 

			SetConsoleTextAttribute(hConsole, 7);
		}
	}




	return 0;
}
