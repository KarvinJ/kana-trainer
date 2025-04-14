#include "fileManager.h"

std::vector<std::string> saveInitialHighScores()
{
    std::vector<std::string> highScores;
    highScores.reserve(9);

    std::ofstream highScoresFile("assets/high-scores.txt");

    std::string name = "aaa";

    for (int i = 8; i > 0; i--)
    {
        std::string scoreString = std::to_string(i * 100);
        std::string fullScoreString = name + "             " + scoreString;

        if (i == 1)
        {
            highScoresFile << fullScoreString;
        }
        else
        {
            highScoresFile << fullScoreString << "\n";
        }

        highScores.push_back(fullScoreString);
    }

    highScoresFile.close();

    return highScores;
}

std::string extractLastNChars(std::string const &str, size_t n)
{
    if (str.size() < n)
    {
        return str;
    }

    return str.substr(str.size() - n);
}

std::vector<HighScore> getFullScoreData(std::vector<std::string> highScores)
{
    std::vector<HighScore> scores;
    scores.reserve(9);

    int rank = 1;

    for (auto &highScore: highScores)
    {
        std::string scoreString = extractLastNChars(highScore, 3);
        int score = stoi(scoreString);

        std::string nameString = highScore.substr(0,3);
        
        scores.push_back({rank, nameString, score});
        rank++;
    }
    
    return scores;
}

std::vector<std::string> saveActualHighScores(std::vector<std::string> highScores, int actualScore, std::string playerName)
{
    std::vector<std::string> actualScores;
    actualScores.reserve(9);

    bool actualScoreWasAdded = false;

    for (size_t i = 0; i < highScores.size(); i++)
    {
        std::string highScore = highScores[i];

        std::string scoreString = extractLastNChars(highScore, 3);

        int score = stoi(scoreString);

        if (!actualScoreWasAdded && actualScore > score)
        {
            std::string fullScore = playerName + "             " + std::to_string(actualScore);
            actualScores.push_back(fullScore);
            actualScoreWasAdded = true;
        }

        actualScores.push_back(highScore);
    }

    //Only update the file if a new score was added.
    if (actualScoreWasAdded)
    {
        //remove the last element aka the lowest score.
        actualScores.pop_back();
    
        std::ofstream highScoresFile("assets/high-scores.txt");
    
        //I can create a infinite loop and it will create a really big file and create a high ram consumption.
        for (size_t i = 0; i < actualScores.size(); i++)
        {
            std::string fullScoreString = actualScores[i];
    
            if (i == actualScores.size() - 1)
            {
                highScoresFile << fullScoreString;
            }
            else
            {
                highScoresFile << fullScoreString << "\n";
            }
        }
    
        highScoresFile.close();

        return actualScores;
    }

    return highScores;
}

std::vector<std::string> loadHighScores()
{
    std::vector<std::string> scores;
    scores.reserve(9);

    std::ifstream highScoresFile("assets/high-scores.txt");

    if (!highScoresFile.is_open())
    {
        return saveInitialHighScores();
    }

    for (std::string line; getline(highScoresFile, line);)
    {
        scores.push_back(line);
    }

    highScoresFile.close();

    return scores;
}

void saveScore(int score)
{
    std::ofstream highScoreFile("assets/high-score.txt");

    highScoreFile << std::to_string(score);

    highScoreFile.close();
}

void savePlayerName(std::string playerName)
{
    std::ofstream playerNameFile("assets/player.txt");

    playerNameFile << playerName;

    playerNameFile.close();
}

std::string loadPlayerName()
{
    std::string playerName;

    std::ifstream playerNameFile("assets/player.txt");

    if (!playerNameFile.is_open())
    {
        savePlayerName("aaa");
        return "aaa";
    }

    getline(playerNameFile, playerName);

    playerNameFile.close();

    return playerName;
}

int loadHighScore()
{
    std::string highScoreText;

    std::ifstream highScoreFile("assets/high-score.txt");

    // if the highscore file doesn't exist just create the file and return 0
    if (!highScoreFile.is_open())
    {
        saveScore(0);
        return 0;
    }

    getline(highScoreFile, highScoreText);

    highScoreFile.close();

    int highScore = stoi(highScoreText);

    return highScore;
}