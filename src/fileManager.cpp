#include "fileManager.h"

std::vector<std::string> saveInitialHighScores()
{
    std::vector<std::string> scores;
    scores.reserve(10);

    std::ofstream highScoresFile("assets/high-scores.txt");

    std::string name = "aaa";

    int placement = 1;
    for (int i = 8; i > 0; i--)
    {
        std::string placementString = std::to_string(placement);
        std::string scoreString = std::to_string(i * 100);

        std::string fullScore = placementString + "                 " + name + "             " + scoreString;

        if (placement == 1)
        {
            fullScore = placementString + "                  " + name + "             " + scoreString;
        }

        placement++;

        if (i == 1)
        {
            highScoresFile << fullScore;
        }
        else
        {
            highScoresFile << fullScore << "\n";
        }

        scores.push_back(fullScore);
    }

    highScoresFile.close();

    return scores;
}

std::string extractLastNChars(std::string const &str, size_t n)
{
    if (str.size() < n)
    {
        return str;
    }

    return str.substr(str.size() - n);
}

std::vector<std::string> saveActualHighScores(std::vector<std::string> highScores, int actualScore, std::string playerName)
{
    int totalHighScores = highScores.size() - 1;

    int currentScoreIndex = 0;

    for (int i = 0; i <= totalHighScores; i++)
    {
        std::string highScore = highScores[i];

        std::string scoreString = extractLastNChars(highScore, 3);

        int score = stoi(scoreString);

        if (actualScore > score)
        {
            currentScoreIndex = i;
            break;
        }
    }

    std::vector<std::string> scores;
    scores.reserve(10);

    std::ofstream highScoresFile("assets/high-scores.txt");

    for (int i = 0; i <= totalHighScores; i++)
    {
        std::string placementString = std::to_string(i + 1);
        std::string scoreString = std::to_string(i * 100);
        std::string name = "aaa";

        std::string fullScoreString = highScores[i];

        if (i == currentScoreIndex)
        {
            scoreString = std::to_string(actualScore);
            name = playerName;
            fullScoreString = placementString + "                 " + name + "             " + scoreString;
        }

        if (i == 0)
        {
            std::string scoreString = extractLastNChars(fullScoreString, 3);
            fullScoreString = placementString + "                  " + name + "             " + scoreString;
        }

        if (i == totalHighScores)
        {
            highScoresFile << fullScoreString;
        }
        else
        {
            highScoresFile << fullScoreString << "\n";
        }

        scores.push_back(fullScoreString);
    }

    highScoresFile.close();

    return scores;
}

std::vector<std::string> loadHighScores()
{
    std::vector<std::string> scores;
    scores.reserve(10);

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