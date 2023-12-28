#include<iostream>
#include<string>
#include<vector>
#include<fstream>
using namespace std;
// vector nhan
vector<string> dsNhan;
// vector tap du lieu
vector<vector<string> > dsMau;
// danh sach cac dac trung, moi dac trung chua danh dach cac gia tri
vector<vector<string> > dsDactrung;

vector<string> splitString(string s);
class Lable {
public:
	int id;
	int slMau = 0;
};
class Values {
public:
	int soMau = 0;
	string ten;
	vector<Lable> Lables;
};
class Feature {
public:
	vector<Values> valuesList;
	double IG;
	int soMau;
	int id;
	string giaTri;
};
// ham tao features tu ds dac trung. // tao cho node ban dau
vector<Feature> createFeatures();
class Node {
public:
	vector<vector<string> > tapDL;
	Node* parent;
	vector<Feature> features;
	Feature feature;
	string tenNhan;
	string giaTriMaNoDuocPhanLoai;// moi nut cha se chia lam nhieu nhanh 
	// gia tri, moi nhanh se co nut con tuong ung, ta can luu lai nut con
	//do duoc sinh ra boi nhanh nao cua nut cha

};
class Kq {
public:
	string tenNhan;
	vector<Node*> dsNode;
	Kq(string tenNhan) {
		this->tenNhan = tenNhan;
	}
};
// danh sach ket qua
vector<Kq> dsKq;
void createNode(string tenValues, Node* parent) {

	Node* node = new Node();
	node->giaTriMaNoDuocPhanLoai = tenValues;
	if (parent != NULL) {
		// do du lieu vao node, lay du lieu tu node cha.
		for (int i = 0; i < parent->tapDL.size(); i++) {
			if (parent->tapDL.at(i).at(parent->feature.id) == tenValues) {
				node->tapDL.push_back(parent->tapDL.at(i));
			}
		}
		// tao tap dac trung
		for (int i = 0; i < parent->features.size(); i++) {
			if (parent->feature.id != parent->features.at(i).id)
				node->features.push_back(parent->features.at(i));
		}
	}
	else {// do du lieu vao node, lay du lieu tu tap goc,
		  //tap feature duoc tao ban dau
		node->tapDL = dsMau;
		node->features = createFeatures();
	}
	if (node->tapDL.size() != 0) {// du dieu kien thi moi thuc hien phan sau
		// set nut cha
		node->parent = parent;
		// duyet tap dac trung reset du lieu
		for (int i = 0; i < node->features.size(); i++) {
			node->features.at(i).soMau = 0;
			int soGiaTriTrongDacTrung = node->features.at(i).valuesList.size();
			for (int j = 0; j < soGiaTriTrongDacTrung; j++) {
				node->features.at(i).valuesList.at(j).soMau = 0;
				int soNhan = node->features.at(i).valuesList.at(j).Lables.size();
				for (int k = 0; k < soNhan; k++) {
					node->features.at(i).valuesList.at(j).Lables.at(k).slMau = 0;
				}
			}
		}
		// do du lieu vao tap dac trung
		for (int i = 0; i < node->tapDL.size(); i++) {
			for (int j = 0; j < node->features.size(); j++) {
				int soGiaTriTrongDacTrung = node->features.at(j)
					.valuesList.size();
				for (int k = 0; k < soGiaTriTrongDacTrung; k++) {
					for (int l = 0; l < dsNhan.size(); l++) {
						node->features.at(j).valuesList.at(k)
										.Lables.at(l).slMau++;
						node->features.at(j).valuesList.at(k).soMau++;
						node->features.at(j).soMau++;
					}
				}
			}
		}
		// kiem tra tap du lieu co dong nhat hay khong
		bool dongNhat = true;
		if (node->tapDL.size() > 0) {
			int idOfLable = node->tapDL.at(0).size() - 1;
			string mLableOfSampleFirst = node->tapDL.at(0).at(idOfLable);
			for (int i = 0; i < node->tapDL.size(); i++) {
				if (mLableOfSampleFirst != node->tapDL.at(i).at(idOfLable)) {
					dongNhat = false; break;
				}
			}
		}

		//
		if (dongNhat) {
			node->tenNhan = node->tapDL.at(0).at(node->tapDL.at(0).size() - 1);
			bool daco = false;
			for (int i = 0; i < dsKq.size(); i++) {
				if (dsKq.at(i).tenNhan == node->tenNhan) {
					daco = true;
					dsKq.at(i).dsNode.push_back(node);
					break;
				}
			}
			if (!daco) {
				Kq kq(node->tenNhan); kq.dsNode.push_back(node);
				dsKq.push_back(kq);
			}
		}
		else if (node->features.size() > 0) {
			// tinh IG cho cac dac trung IG = H(s) - entropy
			  // tinh H(s)
			double Hs = 0;
			vector<double> dsSoLuongMauMoiNhan;// dem so luong moi nhan,
			//xong luu vao danh sach nay
			// thu tu trong danh sach nay trung voi thu tu trong tap nhan
			// khoi tao gia tri 0 cho moi nhan
			for (int i = 0; i < dsNhan.size(); i++) {
				dsSoLuongMauMoiNhan.push_back(0);
			}
			//cout << dsSoLuongMauMoiNhan.size();
			for (int i = 0; i < node->features.size(); i++) {
				int soGiaTriCuaMoiDacTrung = node->features.at(i)
					.valuesList.size();
				for (int j = 0; j < soGiaTriCuaMoiDacTrung; j++) {
					int soNhan = node->features.at(i)
							.valuesList.at(j).Lables.size();
					for (int k = 0; k < soNhan; k++) {
						Lable tmpLable = node->features
							.at(i).valuesList.at(j).Lables.at(k);
						dsSoLuongMauMoiNhan.at(tmpLable.id) += tmpLable.slMau;
					}
				}
			}
			// tinh tong so mau
			long sumMau = 0;
			for (int i = 0; i < dsSoLuongMauMoiNhan.size(); i++) {
				sumMau += dsSoLuongMauMoiNhan.at(i);
			}
			// tinh xac xuat cho moi nhan
			for (int i = 0; i < dsSoLuongMauMoiNhan.size(); i++) {
				dsSoLuongMauMoiNhan.at(i) = dsSoLuongMauMoiNhan.at(i) / sumMau;
			}

			for (int i = 0; i < dsSoLuongMauMoiNhan.size(); i++) {
				double pi = dsSoLuongMauMoiNhan.at(i);
				Hs += -pi * log(pi) / log(2);
			}
			// tinh entropy cho moi dac trung
			vector<double> dsEntropy;
			for (int i = 0; i < node->features.size(); i++) {
				double sxCuaMoiDacTrung = 0;
				int soGiaTri = node->features.at(i).valuesList.size();
				for (int j = 0; j < soGiaTri; j++) {
					double tongSxCuaMotGiaTri = 0;
					int soNhan = node->features.at(i)
						.valuesList.at(j).Lables.size();
					for (int k = 0; k < soNhan; k++) {
						double xsMoiNhanTrongThuocTinh =
						node->features.at(i).valuesList.at(j).Lables.at(k).slMau 
							/ node->features.at(i).valuesList.at(j).soMau;
						tongSxCuaMotGiaTri += -xsMoiNhanTrongThuocTinh
							* log(xsMoiNhanTrongThuocTinh) / log(2);
					}
					sxCuaMoiDacTrung += tongSxCuaMotGiaTri *
						(node->features.at(i).valuesList.at(j).soMau
								/ node->features.at(i).soMau);
				}
				dsEntropy.push_back(sxCuaMoiDacTrung);
			}
			// tinh IG, tim IG max
			double igMax = -1; int idOfIGMax = 0;
			for (int i = 0; i < dsEntropy.size(); i++) {
				if (Hs - dsEntropy.at(i) > igMax) {
					igMax = Hs - dsEntropy.at(i);
					idOfIGMax = i;
				}
			}
			// gan dac trung co IG max cho node
			node->feature = node->features.at(idOfIGMax); // dong nay se gap
														//loi neu features rong
			// duyet gia tri cua dac trung, tiep tuc tao ra cac nut con
			//cout<< node->feature.valuesList.size()<<" ";
			for (int i = 0; i < node->feature.valuesList.size(); i++) {
				//cout << i << " "; cout << node->feature.giaTri << " ";
				createNode(node->feature.valuesList.at(i).ten, node);
			}
		}
	}
}
// ham tra ve ket qua phan lop cua benh nhan moi nhap vao
string ketQuaPhanLop(vector<string> duLieuBenhNhan, vector<Kq> dsKq);
int main() {

	ifstream iFile;
	iFile.open("data_huan_luyen.txt");
	// doc danh sach nhan
	string dsNhanString;  getline(iFile, dsNhanString);
	dsNhan = splitString(dsNhanString);
	// doc danh sach dac trung
	int soDT; string tmp; iFile >> soDT; iFile.ignore();
	for (int i = 0; i < soDT; i++) {
		getline(iFile, tmp);
		dsDactrung.push_back(splitString(tmp));
	}
	// doc danh sach mau
	int soMau; iFile >> soMau; iFile.ignore();
	for (int i = 0; i < soMau; i++) {
		getline(iFile, tmp);
		dsMau.push_back(splitString(tmp));
	}
	iFile.close();
	createNode("", NULL);
	//
	cout << "Tap luat: ";
	for (int i = 0; i < dsKq.size(); i++) {
		cout <<endl<< dsKq.at(i).tenNhan << " neu:";
		string kq;
		for (int j = 0; j < dsKq.at(i).dsNode.size(); j++) {
			Node* node = dsKq.at(i).dsNode.at(j);
			while (node->parent != NULL) {
				if(node->parent->parent == NULL)
					kq = node->parent->feature.giaTri+
					" = " + node->giaTriMaNoDuocPhanLoai + kq;
				else
					kq = " & " + node->parent->feature.giaTri + 
					" = " + node->giaTriMaNoDuocPhanLoai + kq;
				node = node->parent;
			}
			cout << endl << kq;
			if (j != dsKq.at(i).dsNode.size() - 1) cout << " hoac";
			kq = "";
		}
	}
	
	// test voi mau thu nghiem
	vector<string> dsNhanThucTe, dsNhanDuDoan;
	iFile.open("data_thu_nghiem.txt");
	iFile >> soMau; iFile.ignore();
	vector<string> tmpMau;
	for (int i = 0; i < soMau; i++) {
		getline(iFile, tmp);
		tmpMau = splitString(tmp);
		dsNhanThucTe.push_back(tmpMau.at(tmpMau.size() - 1));
		tmpMau.pop_back(); // xoa nhan, danh sach chi con cac dac trung
		dsNhanDuDoan.push_back(ketQuaPhanLop(tmpMau, dsKq));

	}
	
	int tp = 0, fp = 0, fn = 0, tn = 0;
	for (int i = 0; i < soMau; i++) {
		if (dsNhanThucTe.at(i) == "dtd" && dsNhanDuDoan.at(i) == "dtd") {
			tp++;
		}
		if (dsNhanThucTe.at(i) == "dtd" && dsNhanDuDoan.at(i) == "bt") {
			fn++;
		}
		if (dsNhanThucTe.at(i) == "bt" && dsNhanDuDoan.at(i) == "dtd") {
			fp++;
		}
		if (dsNhanThucTe.at(i) == "bt" && dsNhanDuDoan.at(i) == "bt") {
			tn++;
		}
	}
	double recall = (double)tp / (tp + fn);
	double fpRate = (double)fp / (fp + tn);
	double precision = (double)tp / (tp + fp);
	double f1 = (double)2 * recall * precision / (recall + precision);
	double accuracy = (double)(tp + tn) / (tp + fn + fp + tn);
	cout << "\nrecall = " << recall;
	cout << "\nfp rate = " << fpRate;
	cout << "\nprecison = " << precision;
	cout << "\nf1 = " << f1;
	cout << "\naccuracy = " << accuracy;
	
	
	// nhap thong tin benh nhan moi de kiem tra
	/*vector<string> bnCanPhanLop;// vector chua thong tin 
								// benh nhan can kiem tra
	cout << "\nNhap thong tin benh nhan:\n";
	for (int i = 0; i < dsDactrung.size(); i++) {
		cout << dsDactrung.at(i).at(0) << ": ";
		string tmpDacTrung; fflush(stdin); 
		getline(cin, tmpDacTrung);
		bnCanPhanLop.push_back(tmpDacTrung);
	}
	cout << "Ket qua: " << ketQuaPhanLop(bnCanPhanLop, dsKq);*/
	return 0;
}
vector<Feature> createFeatures() {
	vector<Feature> features;
	for (int i = 0; i < dsDactrung.size(); i++) {
		Feature f;
		f.id = i;
		f.giaTri = dsDactrung.at(i).at(0);
		f.soMau = 0;
		// create valuesList
		vector<Values> valuesList;
		for (int j = 1; j < dsDactrung.at(i).size(); j++) {
			Values values;
			values.soMau = 0;
			values.ten = dsDactrung.at(i).at(j);
			// create danh sach nhan
			vector<Lable> lables;
			for (int k = 0; k < dsNhan.size(); k++) {
				Lable l;
				l.id = k;
				l.slMau = 0;
				lables.push_back(l);
			}
			values.Lables = lables;
			valuesList.push_back(values);
		}
		f.valuesList = valuesList;
		features.push_back(f);
	}
	return features;
}
vector<string> splitString(string s) {
	vector<string> result;
	string tmp = "";
	for (int i = 0; i < s.length(); i++) {
		if (s[i] == ' ') {
			result.push_back(tmp); tmp = "";
		}
		else {
			tmp += s[i];
			if (i == s.length() - 1) result.push_back(tmp);
		}
	}
	return result;
}
string ketQuaPhanLop(vector<string> duLieuBenhNhan, vector<Kq> dsKq) {
	for (int i = 0; i < dsKq.size(); i++) {
		Kq kq = dsKq.at(i);
		for (int j = 0; j < kq.dsNode.size(); j++) {
			Node *node = kq.dsNode.at(j);
			while (node->parent != NULL && node->giaTriMaNoDuocPhanLoai 
					== duLieuBenhNhan.at(node->parent->feature.id)) {
				node = node->parent;
			}
			if (node->parent == NULL) return kq.tenNhan;
		}
	}
	return "Khong phan loai duoc! do tap mau con han che.";
}