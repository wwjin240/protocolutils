/*
*  create by wangweijin
*/

#pragma once
//#include "memory/WJS_MemoryPoolManage.h"

#define NOTBALANCEMAXNUM 4

template<typename T,typename D>
class WJS_BBSTree;

//�߶�ƽ�������������ڵ�
template<typename T,typename D>
class WJS_BBSTreeNode
{
	friend class WJS_BBSTree<T,D>;
public:
	WJS_BBSTreeNode();
	~WJS_BBSTreeNode();

	void MakePair(const T& key,const D& data);

	T&    First()
	{
		return m_key;
	}

	D&   Second()
	{
		return m_data;
	}

	//��ע:�����ֵ��Դ�һ��,����ʱ��ӳ�,����ʱ������
	inline bool bBalance()
	{
		if(m_LeftFactor-m_RightFactor>1 || m_LeftFactor-m_RightFactor<-1)
			return false;
		return true;
	}

public:
	D   m_data;
	T   m_key;
protected:
	int  m_LeftFactor;
	int  m_RightFactor;
	int  m_MaxFactor;
//public:
	WJS_BBSTreeNode*  m_pParent;//��
	WJS_BBSTreeNode*  m_pLeftChild;//��
	WJS_BBSTreeNode*  m_pRightChild;//��

//public:
//	WJS_NEW_FROM_MEMORYPOOL
//	WJS_DEL_FROM_MEMORYPOOL
};

template<typename T,typename D>
WJS_BBSTreeNode<T,D>::WJS_BBSTreeNode()
:m_pParent(0)
,m_pLeftChild(0)
,m_pRightChild(0)
,m_LeftFactor(0)
,m_RightFactor(0)
,m_MaxFactor(0)
{

}

template<typename T,typename D>
WJS_BBSTreeNode<T,D>::~WJS_BBSTreeNode()
{

}

template<typename T,typename D>
void WJS_BBSTreeNode<T,D>::MakePair(const T& key,const D& data)
{
	m_key=key;
	m_data=data;
}



//�߶�ƽ������������
//#define NOTBALANCEMAXNUM 4
/////////////////////////////////
template<typename T,typename D>
class WJS_BBSTree
{
public:
	WJS_BBSTree();
	~WJS_BBSTree();

	bool Insert(const T& key,const D& data,bool bOver=false);//bOverָ�Ƿ񸲸���ͬ�Ĺؼ�ֵ

	WJS_BBSTreeNode<T,D>* Search(const T& key);


	bool empty(){return m_RootNode==0;}

	void Free();

	void clear();

	int  size()
    {
		return m_NodeNum;
	}

	bool DeleteKey(const T& key);
	bool DeleteOneNode(WJS_BBSTreeNode<T,D>* pNode);

	WJS_BBSTreeNode<T,D>* begin();//���������,��������������
	WJS_BBSTreeNode<T,D>* next();//ȡ��һ��
	WJS_BBSTreeNode<T,D>* cur();//ɾ��ʱ�������ȡ�ã�����next

protected:
	bool RotateRight(WJS_BBSTreeNode<T,D>* pNode);//��һ���ڵ�����,pNode������һ�����ڵĵ�
	bool RotateLeft(WJS_BBSTreeNode<T,D>* pNode);//��һ���ڵ�����

	void ReBalance();

	void ComputerOfferFactor()
	{
		if(m_RootNode)
			ComputerOfferFactor(m_RootNode);
	}

	void GetSameValueNum(T& key,int &num);
	void GetSameValueNum(WJS_BBSTreeNode<T,D>* pNode,T& key,int &num);

protected:
	int  ComputerOfferFactor(WJS_BBSTreeNode<T,D>* pNode);//���㹱������

	void UpDownOfferFactor(WJS_BBSTreeNode<T,D>* pNode);//�ϴ���������

	void UpDownOfferFactor(WJS_BBSTreeNode<T,D>* pNode,WJS_BBSTreeNode<T,D>* pNode_NotProcess);//�ϴ���������,���Ӳ��ٴ���Ĳ�ƽϡ�ڵ�

	void FreeNode(WJS_BBSTreeNode<T,D>* pNode);
	void InsertNode_AfterNode(WJS_BBSTreeNode<T,D>* pNode,WJS_BBSTreeNode<T,D>* pInsertNode);

	void AddNotBalanceNode(WJS_BBSTreeNode<T,D>* pNode);
	void ResetNotBalanceNode();

	WJS_BBSTreeNode<T,D>* GetLeftLeftNode(WJS_BBSTreeNode<T,D>* pNode);//����һ���ڵ����������յ�

	void UpConvertLeftLeftNode(WJS_BBSTreeNode<T,D>** pLeftLeftNode);//�Ϸ�����ڵ�

	void CreateBeginSearchNode(); //������ʼ�����ڵ�

	

protected:
     WJS_BBSTreeNode<T,D>* m_RootNode;
	 int                   m_NodeNum;

	 WJS_BBSTreeNode<T,D>* m_NotBalanceNode[NOTBALANCEMAXNUM];

	 //�������ڱ���ʱʹ��
	 WJS_BBSTreeNode<T,D>* m_CurSearchNode; //��ǰ�����Ľڵ�


	 enum eNodeSearchType
	 {
		 eNST_None=-1,//��Ч
		 eNST_Ret=0, //��
		 eNST_LeftRet=1, //���
	 };

	 eNodeSearchType      m_CurSearchType;//��ǰ��������

//public:
//	WJS_NEW_FROM_MEMORYPOOL
//	WJS_DEL_FROM_MEMORYPOOL
};

template<typename T,typename D>
WJS_BBSTree<T,D>::WJS_BBSTree()
:m_RootNode(0)
,m_NodeNum(0)
,m_CurSearchNode(0)
,m_CurSearchType(eNST_None)
{
	ResetNotBalanceNode();
}

template<typename T,typename D>
WJS_BBSTree<T,D>::~WJS_BBSTree()
{
	Free();
}

//template<typename T,typename D>
//bool WJS_BBSTree<T,D>::empty()
//{
//	return m_RootNode==0;
//}


template<typename T,typename D>
bool WJS_BBSTree<T,D>::Insert(const T& key,const D& data,bool bOver)
{	
	if(!m_RootNode)
	{
		//����ĵ�һ���ڵ�
		WJS_BBSTreeNode<T,D>* pNode=new WJS_BBSTreeNode<T,D>();
		pNode->MakePair(key,data);
		m_RootNode=pNode;	
		m_NodeNum++;
	}
	else
	{
		WJS_BBSTreeNode<T,D>* pNode=new WJS_BBSTreeNode<T,D>();
		pNode->MakePair(key,data);

		WJS_BBSTreeNode<T,D>* pInsertNode=m_RootNode;
		bool bFound=false;

		while(!bFound)
		{
			if(key<pInsertNode->m_key)
			{
				if(!pInsertNode->m_pLeftChild)
				{
					pInsertNode->m_pLeftChild=pNode;
					pNode->m_pParent=pInsertNode;

					bFound=true;
					m_NodeNum++;


                   UpDownOfferFactor(pNode);
				   ReBalance();
				   return true;
				}
				else
				{
					pInsertNode=pInsertNode->m_pLeftChild;
				}
			}//if
			else if(key>pInsertNode->m_key)
			{
				if(!pInsertNode->m_pRightChild)
				{
					pInsertNode->m_pRightChild=pNode;
					pNode->m_pParent=pInsertNode;
					bFound=true;
					m_NodeNum++;


                    UpDownOfferFactor(pNode);
					ReBalance();
					return true;
				}
				else
				{
					pInsertNode=pInsertNode->m_pRightChild;
				}
			}
			else
			{
				delete pNode;
				//���,������
				if(bOver)
				{
					pInsertNode->m_data=data;
					return true;
				}
				
				return false;
			}
		}//while

	}
	return true;
}

template<typename T,typename D>
WJS_BBSTreeNode<T,D>*  WJS_BBSTree<T,D>::Search(const T& key)
 {
	if(!m_RootNode)
	{
		return 0;
	}

	WJS_BBSTreeNode<T,D>* pSearchNode=m_RootNode;
	while(pSearchNode)
	{
		if(key<pSearchNode->m_key)
		{
			pSearchNode=pSearchNode->m_pLeftChild;
		}
		else if(key>pSearchNode->m_key)
		{
			pSearchNode=pSearchNode->m_pRightChild;
		}
		else
		{
			return pSearchNode;
		}
	}//while
	
	return 0;
 }

template<typename T,typename D>
void WJS_BBSTree<T,D>::Free()
{
	if(m_RootNode)
	{
		FreeNode(m_RootNode);
		m_NodeNum=0;
		ResetNotBalanceNode();
		m_RootNode=0;
		m_CurSearchNode=0;
		m_CurSearchType=eNST_None;


	}
}

template<typename T,typename D>
void WJS_BBSTree<T,D>::clear()
{
	if(m_RootNode)
	{
		FreeNode(m_RootNode);
		m_NodeNum=0;
		ResetNotBalanceNode();
		m_RootNode=0;
		m_CurSearchNode=0;
		m_CurSearchType=eNST_None;


	}
}


template<typename T,typename D>
void WJS_BBSTree<T,D>::FreeNode(WJS_BBSTreeNode<T,D>* pNode)
{
	if(pNode->m_pLeftChild)
	{
		FreeNode(pNode->m_pLeftChild);
	}

	if(pNode->m_pRightChild)
	{
		FreeNode(pNode->m_pRightChild);
	}

	delete pNode;
}

template<typename T,typename D>
bool WJS_BBSTree<T,D>::DeleteOneNode(WJS_BBSTreeNode<T,D>* pNode)
{
	WJS_BBSTreeNode<T,D>* parentNode=pNode->m_pParent;

	//���ر����ĸ���
	if(m_CurSearchNode==pNode)
		next();

	if(parentNode)
	{
		WJS_BBSTreeNode<T,D>* changeNode=0;

		bool bLeft=true;
		if(pNode==parentNode->m_pRightChild)
			bLeft=false;

		if(pNode->m_pLeftChild && pNode->m_pRightChild)
		{
			changeNode=pNode->m_pRightChild;
			if(!changeNode->m_pLeftChild)
			{
				if(bLeft)
				{
					parentNode->m_pLeftChild=changeNode;
				}
				else
				{
					parentNode->m_pRightChild=changeNode;
				}
				changeNode->m_pParent=parentNode;
				changeNode->m_pLeftChild=pNode->m_pLeftChild;
				changeNode->m_pLeftChild->m_pParent=changeNode;
				UpDownOfferFactor(changeNode->m_pLeftChild);
			}
			else
			{
				changeNode->m_pParent=0;
				WJS_BBSTreeNode<T,D>* pLeftLeft=GetLeftLeftNode(changeNode);
				UpConvertLeftLeftNode(&pLeftLeft);
				pLeftLeft->m_pLeftChild=pNode->m_pLeftChild;
				pLeftLeft->m_pLeftChild->m_pParent=pLeftLeft;
				pLeftLeft->m_pParent=parentNode;
				if(bLeft)
				{
					parentNode->m_pLeftChild=pLeftLeft;
				}
				else
				{
					parentNode->m_pRightChild=pLeftLeft;
				}
				UpDownOfferFactor(pLeftLeft->m_pLeftChild);
			}
		}
		else if(pNode->m_pLeftChild)
		{
			changeNode=pNode->m_pLeftChild;
			if(bLeft)
			{
				parentNode->m_pLeftChild=changeNode;
			}
			else
			{
				parentNode->m_pRightChild=changeNode;
			}
			if(changeNode)
			{
				changeNode->m_pParent=parentNode;
			}
			UpDownOfferFactor(changeNode);
		}
		else if(pNode->m_pRightChild)
		{
			changeNode=pNode->m_pRightChild;
			if(bLeft)
			{
				parentNode->m_pLeftChild=changeNode;
			}
			else
			{
				parentNode->m_pRightChild=changeNode;
			}
			if(changeNode)
			{
				changeNode->m_pParent=parentNode;
			}
			UpDownOfferFactor(changeNode);
		}
		else
		{
			if(bLeft)
			{
				parentNode->m_pLeftChild=0;
				parentNode->m_LeftFactor=0;
				parentNode->m_MaxFactor=parentNode->m_RightFactor;
			}
			else
			{
				parentNode->m_pRightChild=0;
				parentNode->m_RightFactor=0;
				parentNode->m_MaxFactor=parentNode->m_LeftFactor;
			}
			UpDownOfferFactor(parentNode);
		}
	
		/////////////////////////

	}
	else
	{
		//ɾ���Ǹ����
		if(pNode->m_pLeftChild && pNode->m_pRightChild)
		{
			WJS_BBSTreeNode<T,D>* changeNode=pNode->m_pRightChild;
			if(!changeNode->m_pLeftChild)
			{
				changeNode->m_pParent=0;
				changeNode->m_pLeftChild=pNode->m_pLeftChild;
				changeNode->m_pLeftChild->m_pParent=changeNode;
				m_RootNode=changeNode;
				UpDownOfferFactor(changeNode->m_pLeftChild);
			}
			else
			{
				changeNode->m_pParent=0;

				WJS_BBSTreeNode<T,D>* pLeftLeft=GetLeftLeftNode(changeNode);
				UpConvertLeftLeftNode(&pLeftLeft);
				pLeftLeft->m_pLeftChild=pNode->m_pLeftChild;
				pLeftLeft->m_pLeftChild->m_pParent=pLeftLeft;
				pLeftLeft->m_pParent=0;
				m_RootNode=pLeftLeft;
				UpDownOfferFactor(pLeftLeft->m_pLeftChild);
			}		
		}
		else if(pNode->m_pLeftChild)
		{
			m_RootNode=m_RootNode->m_pLeftChild;
			m_RootNode->m_pParent=0;
		}
		else if(pNode->m_pRightChild)
		{
			m_RootNode=m_RootNode->m_pRightChild;
			m_RootNode->m_pParent=0;
		}
		else
		{
			m_RootNode=0;
		}
	}

	delete pNode;
	m_NodeNum--;
	ReBalance();
	return true;
}

template<typename T,typename D>
void WJS_BBSTree<T,D>::InsertNode_AfterNode(WJS_BBSTreeNode<T,D>* pNode,WJS_BBSTreeNode<T,D>* pInsertNode)
{
	WJS_BBSTreeNode<T,D>* PP=pNode;
	while(true)
	{
		if(pInsertNode->m_key < PP->m_key)
		{
			if(PP->m_pLeftChild)
			{
			  PP=PP->m_pLeftChild;
			}
			else
			{
				PP->m_pLeftChild=pInsertNode;
				pInsertNode->m_pParent=PP;
				UpDownOfferFactor(pInsertNode);
				return;
			}
		}
		else
		{
			if(PP->m_pRightChild)
			{
				PP=PP->m_pRightChild;
			}
			else
			{
				PP->m_pRightChild=pInsertNode;
				pInsertNode->m_pParent=PP;
				UpDownOfferFactor(pInsertNode);
				return;
			}	
		}
	}
}

template<typename T,typename D>
bool WJS_BBSTree<T,D>::DeleteKey(const T& key)
{
	WJS_BBSTreeNode<T,D>* pNode=Search(key);

	if(pNode)
	{
         return DeleteOneNode(pNode);
	}
	
	return false;
}

//��������Ϊ���ӵ��ҽڵ�,���ӵ��Һ�����Ϊ�Լ�������
//�����ӵ��Һ���Ϊ��,���Լ���Ϊ���Һ��ӵ���ڵ�
//������������ɱ���˳����
template<typename T,typename D>
bool WJS_BBSTree<T,D>::RotateRight(WJS_BBSTreeNode<T,D>* pNode)
{
	WJS_BBSTreeNode<T,D>* pLeftChild=pNode->m_pLeftChild;

	if(!pLeftChild)
		return false;

	//��ת,�ж�����


	pNode->m_pLeftChild=pLeftChild->m_pRightChild;
	if(pNode->m_pLeftChild)
		pNode->m_pLeftChild->m_pParent=pNode;


	pLeftChild->m_pParent=pNode->m_pParent;
	if(pLeftChild->m_pParent)
	{
		if(pLeftChild->m_pParent->m_pLeftChild==pNode)
	        pLeftChild->m_pParent->m_pLeftChild=pLeftChild;
		else
			pLeftChild->m_pParent->m_pRightChild=pLeftChild;

	}

	pLeftChild->m_pRightChild=pNode;
	pNode->m_pParent=pLeftChild;

	if(!pLeftChild->m_pParent)
	{
		//������ڵ�
		m_RootNode=pLeftChild;
	}

	//���㲿������
	//pLeftChild�󲻱�,pNode��û��
	if(pNode->m_pLeftChild)
	{
		UpDownOfferFactor(pNode->m_pLeftChild,pLeftChild);
	}
	else
	{
		pNode->m_LeftFactor=0;
		pNode->m_MaxFactor=pNode->m_RightFactor;
		UpDownOfferFactor(pNode,pLeftChild);

	}
	
	return true;

}

template<typename T,typename D>
bool WJS_BBSTree<T,D>::RotateLeft(WJS_BBSTreeNode<T,D>* pNode)
{
	WJS_BBSTreeNode<T,D>* pRightChild=pNode->m_pRightChild;
	if(!pRightChild)
		return false;

	//����,�ж�����
	//��������Ϊ������������

	pNode->m_pRightChild=pRightChild->m_pLeftChild;
	if(pNode->m_pRightChild)
	{
		pNode->m_pRightChild->m_pParent=pNode;
	}

	pRightChild->m_pParent=pNode->m_pParent;
	
	if(pRightChild->m_pParent)
	{
		//ԭ�ڵ���������
		if(pRightChild->m_pParent->m_pRightChild==pNode)
	      pRightChild->m_pParent->m_pRightChild=pRightChild;
		else
		  pRightChild->m_pParent->m_pLeftChild=pRightChild;
	}

	pRightChild->m_pLeftChild=pNode;
	pNode->m_pParent=pRightChild;

	//���ڵ�ı�
	if(!pRightChild->m_pParent)
	{
		m_RootNode=pRightChild;
	}

	if(pNode->m_pRightChild)
	{
		UpDownOfferFactor(pNode->m_pRightChild,pRightChild);
	}
	else
	{
		pNode->m_RightFactor=0;
		pNode->m_MaxFactor=pNode->m_LeftFactor;
		UpDownOfferFactor(pNode,pRightChild);
	}
	return true;
}


template<typename T,typename D>
void WJS_BBSTree<T,D>::ReBalance()
{
	for(int i=0;i<NOTBALANCEMAXNUM;i++)
	{
		if(m_NotBalanceNode[i])
		{
			if(!m_NotBalanceNode[i]->bBalance())
			{
				if(m_NotBalanceNode[i]->m_LeftFactor<m_NotBalanceNode[i]->m_RightFactor)
				{

					//Ϊ���һ�ַ�����ת���⣬����ʱ��������ҽڵ���ڣ�������ϵ��>��ϵ�����ȶ�������
					if(m_NotBalanceNode[i]->m_pRightChild &&  
						(m_NotBalanceNode[i]->m_pRightChild->m_LeftFactor>m_NotBalanceNode[i]->m_pRightChild->m_RightFactor))
					{
						RotateRight(m_NotBalanceNode[i]->m_pRightChild);
					}
					RotateLeft(m_NotBalanceNode[i]);
				}
				else 
				{
					if(m_NotBalanceNode[i]->m_pLeftChild &&  
						(m_NotBalanceNode[i]->m_pLeftChild->m_RightFactor>m_NotBalanceNode[i]->m_pLeftChild->m_LeftFactor))
					{
						RotateLeft(m_NotBalanceNode[i]->m_pLeftChild);
					}
					RotateRight(m_NotBalanceNode[i]);
				}
			}

			m_NotBalanceNode[i]=0;
		}
	}

	//fixed ����ƽ�����⣬��168,320�ٲ���200
	for(int i=0;i<NOTBALANCEMAXNUM;i++)
	{
		m_NotBalanceNode[i]=0;
	}
}



template<typename T,typename D>
int  WJS_BBSTree<T,D>::ComputerOfferFactor(WJS_BBSTreeNode<T,D>* pNode)
{
	int leftOffer=0;
	int rightOffer=0;

	if(pNode->m_pLeftChild)
	{
		leftOffer=ComputerOfferFactor(pNode->m_pLeftChild)+1;
	}
    
	if(pNode->m_pRightChild)
	{
		rightOffer=ComputerOfferFactor(pNode->m_pRightChild)+1;
	}

	pNode->m_LeftFactor=leftOffer;
	pNode->m_RightFactor=rightOffer;
	pNode->m_MaxFactor=leftOffer>rightOffer ? leftOffer : rightOffer;

	return pNode->m_MaxFactor;
}

template<typename T,typename D>
void WJS_BBSTree<T,D>::UpDownOfferFactor(WJS_BBSTreeNode<T,D>* pNode)
{
	WJS_BBSTreeNode<T,D>* pParent=pNode->m_pParent;
	WJS_BBSTreeNode<T,D>* pCurNode=pNode;

	bool bEnd=false;

	while(!bEnd && pParent)
	{
		if(pCurNode==pParent->m_pLeftChild)
		{
			pParent->m_LeftFactor=pCurNode->m_MaxFactor+1;
		}//
		else
		{
			pParent->m_RightFactor=pCurNode->m_MaxFactor+1;
		}//else

		int tempMaxFactor=pParent->m_LeftFactor > pParent->m_RightFactor ? pParent->m_LeftFactor  : pParent->m_RightFactor;
		if(tempMaxFactor!=pParent->m_MaxFactor)
		{
			pParent->m_MaxFactor=tempMaxFactor;

			if(!pParent->bBalance())
			{
				AddNotBalanceNode(pParent);
			}
			//�����Ϲ�
			pCurNode=pParent;
			pParent=pParent->m_pParent;
		}
		else
		{
			//˵�������ٶ�����Ľڵ��ƽ���Բ���Ӱ��
			bEnd=true;
		}
	}

	return;
}

template<typename T,typename D>
void WJS_BBSTree<T,D>::UpDownOfferFactor(WJS_BBSTreeNode<T,D>* pNode,WJS_BBSTreeNode<T,D>* pNode_NotProcess)
{
	WJS_BBSTreeNode<T,D>* pParent=pNode->m_pParent;
	WJS_BBSTreeNode<T,D>* pCurNode=pNode;

	bool bEnd=false;

	while(!bEnd && pParent)
	{
		if(pCurNode==pParent->m_pLeftChild)
		{
			pParent->m_LeftFactor=pCurNode->m_MaxFactor+1;
		}//
		else
		{
			pParent->m_RightFactor=pCurNode->m_MaxFactor+1;
		}//else

		int tempMaxFactor=pParent->m_LeftFactor > pParent->m_RightFactor ? pParent->m_LeftFactor  : pParent->m_RightFactor;
		if(tempMaxFactor!=pParent->m_MaxFactor)
		{
			pParent->m_MaxFactor=tempMaxFactor;

			if(!pParent->bBalance() && (pParent!=pNode_NotProcess))
			{
				AddNotBalanceNode(pParent);
			}
			//�����Ϲ�
			pCurNode=pParent;
			pParent=pParent->m_pParent;
		}
		else
		{
			//˵�������ٶ�����Ľڵ��ƽ���Բ���Ӱ��
			bEnd=true;
		}
	}

	return;

}
template<typename T,typename D>
void WJS_BBSTree<T,D>::AddNotBalanceNode(WJS_BBSTreeNode<T,D>* pNode)
{
	for(int i=0;i<NOTBALANCEMAXNUM;i++)
	{
		if(!m_NotBalanceNode[i])
		{
			m_NotBalanceNode[i]=pNode;
			return;
		}
	}
}

template<typename T,typename D>
void WJS_BBSTree<T,D>::ResetNotBalanceNode()
{
	for(int i=0;i<NOTBALANCEMAXNUM;i++)
	{
		m_NotBalanceNode[i]=0;
	}
}

template<typename T,typename D>
void WJS_BBSTree<T,D>::GetSameValueNum(WJS_BBSTreeNode<T,D>* pNode,T& key,int &num)
{
	if(pNode->m_key==key)
	{
		num++;
	}

	if(pNode->m_pLeftChild)
	{
		GetSameValueNum(pNode->m_pLeftChild,key,num);
	}

	if(pNode->m_pRightChild)
	{
		GetSameValueNum(pNode->m_pRightChild,key,num);
	}

}

template<typename T,typename D>
void WJS_BBSTree<T,D>::GetSameValueNum(T& key,int &num)
{
	if(m_RootNode)
	{
		GetSameValueNum(m_RootNode,key,num);
	}
}

template<typename T,typename D>
WJS_BBSTreeNode<T,D>* WJS_BBSTree<T,D>::GetLeftLeftNode(WJS_BBSTreeNode<T,D>* pNode)
{
	WJS_BBSTreeNode<T,D>* pLeftLeftNode=pNode->m_pLeftChild;

	while(pLeftLeftNode->m_pLeftChild)
	{
		pLeftLeftNode=pLeftLeftNode->m_pLeftChild;
	}

	return pLeftLeftNode;
}

template<typename T,typename D>
void WJS_BBSTree<T,D>::UpConvertLeftLeftNode(WJS_BBSTreeNode<T,D>** pLeftLeftNode)
{
	WJS_BBSTreeNode<T,D>* pA=*pLeftLeftNode;
	WJS_BBSTreeNode<T,D>* pB=pA->m_pParent;
	WJS_BBSTreeNode<T,D>* pC=pA->m_pRightChild;
	WJS_BBSTreeNode<T,D>* pD=pB->m_pParent;

	while(true)
	{
		pB->m_pLeftChild=pC;
		if(pC)
		{
			pC->m_pParent=pB;
		}

		pA->m_pRightChild=pB;
		pB->m_pParent=pA;

		pA->m_pParent=pD;
		if(pD)
		{
			pD->m_pLeftChild=pA;
		}

		UpDownOfferFactor(pB);

		if(pA->m_pParent)
		{
			//�ٴ�ѭ��
			pB=pA->m_pParent;
			pC=pA->m_pRightChild;
			pD=pB->m_pParent;
		}
		else
		{
			*pLeftLeftNode=pA;
			break;
		}
	}

	////���ӱ����ĸ���
	//if((*pLeftLeftNode==m_CurSearchNode)  && m_CurSearchType==eNST_Ret)
	//{
	//	//�������߸�Ϊ������
	//	m_CurSearchType=eNST_LeftRet;
	//}
}


template<typename T,typename D>
void WJS_BBSTree<T,D>::CreateBeginSearchNode()//������ʼ�����ڵ�
{
	m_CurSearchNode=m_RootNode;
	if(!m_CurSearchNode)
	{
		m_CurSearchType=eNST_None;
		return;
	}

	//�޸����ڵ�û����ڵ�����
	if(m_RootNode->m_pLeftChild)
	{
		while(m_CurSearchNode->m_pLeftChild)
		{
			m_CurSearchNode=m_CurSearchNode->m_pLeftChild;
		}
		//m_CurSearchType=eNST_Ret;
		m_CurSearchType=eNST_LeftRet;
	}
	else
	{
		m_CurSearchType=eNST_LeftRet;

	}


	//while(m_CurSearchNode->m_pLeftChild)
	//{
	//	m_CurSearchNode=m_CurSearchNode->m_pLeftChild;
	//}
	//m_CurSearchType=eNST_Ret;
}

template<typename T,typename D>
WJS_BBSTreeNode<T,D>* WJS_BBSTree<T,D>::begin()
{
	CreateBeginSearchNode();
	
	return m_CurSearchNode;
}

template<typename T,typename D>
WJS_BBSTreeNode<T,D>* WJS_BBSTree<T,D>::next()
{
	if(!m_CurSearchNode)
		return 0;

	switch(m_CurSearchType)
	{
	case eNST_Ret://��
		m_CurSearchNode=m_CurSearchNode->m_pParent;
		m_CurSearchType=eNST_LeftRet;
		break;
	case eNST_LeftRet://���,Ӧ���ұ߷���
		{
			//����ҽڵ����
			if(m_CurSearchNode->m_pRightChild)
			{
				m_CurSearchNode=m_CurSearchNode->m_pRightChild;
				//bool bHasLeft=false;
				while(m_CurSearchNode->m_pLeftChild)
				{
					//bHasLeft=true;
					m_CurSearchNode=m_CurSearchNode->m_pLeftChild;
				}
				//if(bHasLeft)
				//{
				//	m_CurSearchType=eNST_Ret;
				//}
				//else
				    m_CurSearchType=eNST_LeftRet;

			}
			else
			{
				//�ص�����
				if(m_CurSearchNode==m_RootNode)
				{
					m_CurSearchNode=0;
					m_CurSearchType=eNST_None;
					return 0;
				}

				WJS_BBSTreeNode<T,D>* pp=m_CurSearchNode->m_pParent;
				while(m_CurSearchNode && pp)
				{
					//��أ������丸
					if(m_CurSearchNode == pp->m_pLeftChild)
					{
						m_CurSearchNode=m_CurSearchNode->m_pParent;
						m_CurSearchType=eNST_LeftRet;
						return m_CurSearchNode;
					}
					else//������
					{
						m_CurSearchNode=m_CurSearchNode->m_pParent;
						pp=m_CurSearchNode->m_pParent;
					}
				}

				//��������
				m_CurSearchNode=0;
				m_CurSearchType=eNST_None;
				return 0;
			}

		}
		break;
	default:
		m_CurSearchNode=0;
		m_CurSearchType=eNST_None;
		return 0;
	//	break;
	}

	return m_CurSearchNode;
}

template<typename T,typename D>
WJS_BBSTreeNode<T,D>* WJS_BBSTree<T,D>::cur()
{
	return m_CurSearchNode;
}

