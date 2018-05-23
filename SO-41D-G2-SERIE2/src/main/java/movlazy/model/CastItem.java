package movlazy.model;

import java.util.concurrent.CompletableFuture;
import java.util.function.Supplier;

/**
 * @author Miguel Gamboa
 *         created on 04-08-2017
 */
public class CastItem {
    private final int id;
    private final int movieId;
    private final String character;
    private final String name;
    private final Supplier<Actor> actor;

    public CastItem(int id, int movieId, String character, String name, Supplier<Actor> actor) {
        this.id = id;
        this.movieId = movieId;
        this.character = character;
        this.name = name;
        this.actor = actor;
    }

    public int getId() {
        return id;
    }

    public String getCharacter() {
        return character;
    }

    public String getName() {
        return name;
    }

    public int getMovieId() {
        return movieId;
    }

    public Actor getActor() {
        return actor.get();
    }

    @Override
    public String toString() {
        return "CastItem{" +
                "id=" + id +
                ", movieId=" + movieId +
                ", character='" + character + '\'' +
                ", name='" + name + '\'' +
                ", getPersonCreditsCast=" + actor +
                '}';
    }
}
