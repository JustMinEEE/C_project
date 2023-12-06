import requests
from bs4 import BeautifulSoup

while True:
    cho = input("\n초성 입력: ")  # 초성 입력 받아서
    url = f"https://wordrow.kr/%EC%B4%88%EC%84%B1/{cho}/"  # 초성 입력하면 해당하는 단어 모두 찾아주는 사이트 url에 삽입

    response = requests.get(url)  # 사이트에 응답 요청

    soup = BeautifulSoup(response.content, "html.parser")  # BeautifulSoup라이브러리 이용하여 html내용 파싱

    # html코드에서 div(class="content") - section - div(class="larger") - ul - li - a(class="list_link") - b 경로의 줄 가져오기
    word_elements = soup.select("div.content section div.larger ul li a.list_link b")
        
    words = [word.get_text(strip=True) for word in word_elements]  # 가져온 줄에서 단어만 추출
    print(cho, end=' ')

    for word in words:
        if len(word) > len(cho):  # 가져온 단어 사이에 띄어쓰기가 있으면
            word = ''.join(word.split())  # 삭제
        print(word, end=' ')
    print('')